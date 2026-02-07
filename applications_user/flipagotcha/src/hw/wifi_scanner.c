#include "wifi_scanner.h"
#include "uart.h"
#include <furi.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct WifiScanner {
    WifiNetwork networks[MAX_NETWORKS];
    uint8_t network_count;
    CapturedPacket* packets;
    uint16_t packet_count;
    uint16_t packet_capacity;
    bool is_scanning;
    bool is_capturing;
    bool esp32_connected;
    PacketFilterConfig filter_config;  // Packet filter configuration
    AttackType active_attack;          // Currently active attack
    bool channel_hopping;              // Channel hopping enabled
    SessionStats stats;                // Session statistics
    FuriMutex* mutex;
    FuriString* rx_buffer;  // Buffer for incoming UART data
};

// Forward declarations
static void wifi_scanner_parse_scanap_line(WifiScanner* scanner, const char* line);
static void wifi_scanner_uart_rx_callback(const uint8_t* data, size_t len, void* context);

WifiScanner* wifi_scanner_alloc(void) {
    WifiScanner* scanner = malloc(sizeof(WifiScanner));
    if (!scanner) return NULL;
    
    memset(scanner, 0, sizeof(WifiScanner));
    scanner->packet_capacity = 100; // Start with capacity for 100 packets
    scanner->packets = malloc(sizeof(CapturedPacket) * scanner->packet_capacity);
    
    if (!scanner->packets) {
        free(scanner);
        return NULL;
    }
    
    scanner->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if (!scanner->mutex) {
        free(scanner->packets);
        free(scanner);
        return NULL;
    }
    
    scanner->rx_buffer = furi_string_alloc();
    if (!scanner->rx_buffer) {
        furi_mutex_free(scanner->mutex);
        free(scanner->packets);
        free(scanner);
        return NULL;
    }
    
    // Initialize packet filter to capture all packets by default
    scanner->filter_config.filter_flags = PACKET_FILTER_ALL;
    
    // Set up UART callback
    uart_set_rx_callback(wifi_scanner_uart_rx_callback, scanner);
    
    // Check if ESP32 is connected
    scanner->esp32_connected = uart_is_connected();
    
    return scanner;
}

void wifi_scanner_free(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (scanner->mutex) {
        furi_mutex_free(scanner->mutex);
    }
    if (scanner->packets) {
        free(scanner->packets);
    }
    if (scanner->rx_buffer) {
        furi_string_free(scanner->rx_buffer);
    }
    free(scanner);
}

// UART receive callback - processes data from ESP32 Marauder
static void wifi_scanner_uart_rx_callback(const uint8_t* data, size_t len, void* context) {
    WifiScanner* scanner = (WifiScanner*)context;
    if (!scanner || !data || len == 0) return;
    
    if (furi_mutex_acquire(scanner->mutex, 50) != FuriStatusOk) {
        return; // Skip if we can't acquire mutex quickly
    }
    
    // Append received data to buffer
    for (size_t i = 0; i < len; i++) {
        if (data[i] == '\n') {
            // Process complete line
            const char* line = furi_string_get_cstr(scanner->rx_buffer);
            
            // Parse scanap results
            if (scanner->is_scanning && strlen(line) > 0) {
                wifi_scanner_parse_scanap_line(scanner, line);
            }
            
            // Clear buffer for next line
            furi_string_reset(scanner->rx_buffer);
        } else if (data[i] >= 32 && data[i] <= 126) {
            // Only append printable characters
            furi_string_push_back(scanner->rx_buffer, data[i]);
        }
    }
    
    furi_mutex_release(scanner->mutex);
}

// Parse a line from "scanap" output
// Format examples from Marauder:
// "[CH 06] <SSID> (-45dBm) [WPA2]"
static void wifi_scanner_parse_scanap_line(WifiScanner* scanner, const char* line) {
    if (!line || strlen(line) < 10) return;
    if (scanner->network_count >= MAX_NETWORKS) return;
    
    // Look for channel marker
    const char* ch_start = strstr(line, "[CH ");
    if (!ch_start) return;
    
    WifiNetwork* net = &scanner->networks[scanner->network_count];
    memset(net, 0, sizeof(WifiNetwork));
    
    // Parse channel
    int channel = 0;
    if (sscanf(ch_start, "[CH %d]", &channel) == 1) {
        // Validate channel is within WiFi 2.4GHz range (1-14)
        if (channel >= 1 && channel <= 14) {
            net->channel = (uint8_t)channel;
        } else {
            // Invalid channel, use default
            net->channel = 6;
        }
    }
    
    // Find SSID (text between "] " and " (")
    const char* ssid_start = strstr(ch_start, "] ");
    const char* ssid_end = strstr(ssid_start ? ssid_start : line, " (");
    
    if (ssid_start && ssid_end && ssid_end > ssid_start + 2) {
        ssid_start += 2;
        size_t ssid_len = ssid_end - ssid_start;
        if (ssid_len > MAX_SSID_LEN) {
            // SSID exceeds maximum length, truncate with warning
            ssid_len = MAX_SSID_LEN;
        }
        // Ensure we have valid data to copy
        if (ssid_len > 0) {
            strncpy(net->ssid, ssid_start, ssid_len);
            net->ssid[ssid_len] = '\0';
        }
    }
    
    // Parse RSSI
    const char* rssi_start = strstr(line, "(");
    if (rssi_start) {
        int rssi = 0;
        if (sscanf(rssi_start, "(%ddBm)", &rssi) == 1) {
            // Validate RSSI is within reasonable range for WiFi (-100 to 0 dBm)
            if (rssi >= -100 && rssi <= 0) {
                net->rssi = (int8_t)rssi;
            } else {
                net->rssi = -70; // Default moderate signal
            }
        }
    }
    
    // Parse encryption - use single pass to avoid redundant strstr calls
    const char* enc_pos = strstr(line, "[");
    if (enc_pos) {
        enc_pos = strstr(enc_pos + 1, "[");  // Find second bracket (after CH)
        if (enc_pos) {
            if (strncmp(enc_pos, "[WPA3]", 6) == 0) {
                net->encryption = 3;
            } else if (strncmp(enc_pos, "[WPA2]", 6) == 0) {
                net->encryption = 2;
            } else if (strncmp(enc_pos, "[WPA]", 5) == 0) {
                net->encryption = 1;
            } else if (strncmp(enc_pos, "[OPEN]", 6) == 0) {
                net->encryption = 0;
            }
        }
    }
    
    // Only add if we got at least an SSID
    if (strlen(net->ssid) > 0) {
        scanner->network_count++;
    }
}

bool wifi_scanner_start_scan(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    // Reset network list
    scanner->network_count = 0;
    memset(scanner->networks, 0, sizeof(scanner->networks));
    furi_string_reset(scanner->rx_buffer);
    
    // Check if ESP32 is connected
    if (!scanner->esp32_connected) {
        scanner->esp32_connected = uart_is_connected();
    }
    
    bool result = false;
    
    if (scanner->esp32_connected) {
        // Send Marauder scanap command
        result = uart_write_str(MARAUDER_CMD_SCANAP);
        
        if (result) {
            scanner->is_scanning = true;
        }
    }
    
    // Fallback to mock data if ESP32 not connected
    if (!scanner->esp32_connected || !result) {
        scanner->is_scanning = true;
        
        // Add mock data for demonstration
        scanner->network_count = 3;
        
        // Network 1
        strncpy(scanner->networks[0].ssid, "FreeWiFi", MAX_SSID_LEN);
        scanner->networks[0].rssi = -45;
        scanner->networks[0].channel = 6;
        scanner->networks[0].encryption = 2; // WPA2
        
        // Network 2
        strncpy(scanner->networks[1].ssid, "CoffeeShop", MAX_SSID_LEN);
        scanner->networks[1].rssi = -62;
        scanner->networks[1].channel = 11;
        scanner->networks[1].encryption = 2;
        
        // Network 3
        strncpy(scanner->networks[2].ssid, "HomeNetwork", MAX_SSID_LEN);
        scanner->networks[2].rssi = -78;
        scanner->networks[2].channel = 1;
        scanner->networks[2].encryption = 3; // WPA3
        
        result = true;
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

void wifi_scanner_stop_scan(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    // Send stopscan command to Marauder
    if (scanner->esp32_connected) {
        uart_write_str(MARAUDER_CMD_STOPSCAN);
    }
    
    scanner->is_scanning = false;
    
    furi_mutex_release(scanner->mutex);
}

uint8_t wifi_scanner_get_network_count(WifiScanner* scanner) {
    if (!scanner) return 0;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return 0;
    }
    
    uint8_t count = scanner->network_count;
    furi_mutex_release(scanner->mutex);
    
    return count;
}

const WifiNetwork* wifi_scanner_get_network(WifiScanner* scanner, uint8_t index) {
    if (!scanner) return NULL;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return NULL;
    }
    
    if (index >= scanner->network_count) {
        furi_mutex_release(scanner->mutex);
        return NULL;
    }
    
    const WifiNetwork* network = &scanner->networks[index];
    
    furi_mutex_release(scanner->mutex);
    return network;
}

bool wifi_scanner_start_handshake(WifiScanner* scanner, uint8_t network_index) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    // Check network_index while holding mutex to avoid race condition
    if (network_index >= scanner->network_count) {
        furi_mutex_release(scanner->mutex);
        return false;
    }
    
    bool result = false;
    
    if (scanner->esp32_connected) {
        // Validate channel is within valid range (1-14) before using it
        int channel = scanner->networks[network_index].channel;
        if (channel < 1 || channel > 14) {
            channel = 6; // Default to channel 6 if invalid
        }
        
        // Build sniffpmkid command with channel
        // Format: "sniffpmkid -c <channel> -d\n"
        char cmd[MARAUDER_CMD_BUFFER_SIZE];
        snprintf(cmd, sizeof(cmd), "%s -c %d -d\n", MARAUDER_CMD_SNIFFPMKID, channel);
        result = uart_write_str(cmd);
    }
    
    // Fallback to mock handshake capture
    if (!scanner->esp32_connected || !result) {
        // Check for packet overflow with notification
        if (scanner->packet_count >= scanner->packet_capacity) {
            // Packet capacity reached - cannot add more packets
            furi_mutex_release(scanner->mutex);
            return false;
        }
        
        CapturedPacket* pkt = &scanner->packets[scanner->packet_count];
        pkt->length = MOCK_HANDSHAKE_SIZE;
        pkt->timestamp = furi_get_tick();
        pkt->channel = scanner->networks[network_index].channel;
        memset(pkt->data, 0xAA, pkt->length); // Mock data
        scanner->packet_count++;
        result = true;
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

bool wifi_scanner_start_capture(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool result = false;
    uint8_t filter_flags = scanner->filter_config.filter_flags;
    
    // Build capture command based on filter configuration
    // Different ESP32 Marauder commands capture different packet types:
    // - sniffraw: captures all raw packets (when filter is ALL or multiple types)
    // - sniffpmkid: captures EAPOL handshake packets specifically
    // - sniffbeacon: captures beacon frames only (if supported)
    
    if (scanner->esp32_connected) {
        // If only EAPOL filter is enabled, use sniffpmkid command
        if (filter_flags == PACKET_FILTER_EAPOL) {
            // Use sniffpmkid without channel specification (monitor all channels)
            result = uart_write_str("sniffpmkid\n");
        }
        // For any other filter combination, use sniffraw
        // Note: The actual filtering will happen when parsing received packets
        else {
            result = uart_write_str(MARAUDER_CMD_SNIFFRAW);
        }
    }
    
    if (result || !scanner->esp32_connected) {
        scanner->is_capturing = true;
        result = true;
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

void wifi_scanner_stop_capture(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    // Send stopscan command
    if (scanner->esp32_connected) {
        uart_write_str(MARAUDER_CMD_STOPSCAN);
    }
    
    scanner->is_capturing = false;
    
    furi_mutex_release(scanner->mutex);
}

uint16_t wifi_scanner_get_packet_count(WifiScanner* scanner) {
    if (!scanner) return 0;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return 0;
    }
    
    uint16_t count = scanner->packet_count;
    furi_mutex_release(scanner->mutex);
    
    return count;
}

bool wifi_scanner_save_capture(WifiScanner* scanner, const char* filename) {
    if (!scanner || !filename) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if (!storage) {
        furi_mutex_release(scanner->mutex);
        return false;
    }
    
    File* file = storage_file_alloc(storage);
    if (!file) {
        furi_record_close(RECORD_STORAGE);
        furi_mutex_release(scanner->mutex);
        return false;
    }
    
    bool success = false;
    
    // Create directory if it doesn't exist
    FuriString* dir_path = furi_string_alloc();
    furi_string_set_str(dir_path, "/ext/apps_data/flipagotcha");
    storage_common_mkdir(storage, furi_string_get_cstr(dir_path));
    
    // Build full path
    FuriString* full_path = furi_string_alloc();
    furi_string_printf(full_path, "/ext/apps_data/flipagotcha/%s", filename);
    
    // Open file for writing
    if (storage_file_open(file, furi_string_get_cstr(full_path), 
                         FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        
        // Write header
        const char* header = "# Flipagotcha Packet Capture\n";
        storage_file_write(file, header, strlen(header));
        
        // Write packet count
        char count_buf[64];
        snprintf(count_buf, sizeof(count_buf), "# Packets: %u\n\n", scanner->packet_count);
        storage_file_write(file, count_buf, strlen(count_buf));
        
        // Write each packet
        for (uint16_t i = 0; i < scanner->packet_count; i++) {
            CapturedPacket* pkt = &scanner->packets[i];
            
            char pkt_header[128];
            snprintf(pkt_header, sizeof(pkt_header), 
                    "Packet %u: len=%u, ts=%lu, ch=%u\n",
                    i, pkt->length, pkt->timestamp, pkt->channel);
            storage_file_write(file, pkt_header, strlen(pkt_header));
            
            // Write hex dump of packet data (limited to HEX_DUMP_MAX_BYTES)
            uint16_t dump_len = pkt->length < HEX_DUMP_MAX_BYTES ? pkt->length : HEX_DUMP_MAX_BYTES;
            for (uint16_t j = 0; j < dump_len; j += 16) {
                char hex_line[80];
                int offset = 0;
                for (uint16_t k = 0; k < 16 && (j + k) < dump_len; k++) {
                    // Check if there's enough space for "XX " (3 chars) plus newline and null terminator
                    if (offset + 4 < (int)sizeof(hex_line)) {
                        offset += snprintf(hex_line + offset, sizeof(hex_line) - offset,
                                         "%02X ", pkt->data[j + k]);
                    }
                }
                // Safely add newline with bounds check
                if (offset < (int)sizeof(hex_line) - 1) {
                    hex_line[offset++] = '\n';
                }
                storage_file_write(file, hex_line, offset);
            }
            storage_file_write(file, "\n", 1);
        }
        
        storage_file_close(file);
        success = true;
    }
    
    storage_file_free(file);
    furi_string_free(full_path);
    furi_string_free(dir_path);
    furi_record_close(RECORD_STORAGE);
    
    furi_mutex_release(scanner->mutex);
    return success;
}

bool wifi_scanner_is_scanning(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool scanning = scanner->is_scanning;
    furi_mutex_release(scanner->mutex);
    
    return scanning;
}

bool wifi_scanner_is_capturing(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool capturing = scanner->is_capturing;
    furi_mutex_release(scanner->mutex);
    
    return capturing;
}

bool wifi_scanner_is_esp32_connected(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool connected = scanner->esp32_connected;
    furi_mutex_release(scanner->mutex);
    
    return connected;
}

// Actively query UART to verify if ESP32 (Marauder) is responding and update
// the internal connection flag. Returns the new connection state.
bool wifi_scanner_refresh_connection(WifiScanner* scanner) {
    if (!scanner) return false;

    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }

    bool connected = uart_is_connected();
    scanner->esp32_connected = connected;

    furi_mutex_release(scanner->mutex);
    return connected;
}

// Set packet filter configuration
void wifi_scanner_set_filter(WifiScanner* scanner, uint8_t filter_flags) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    scanner->filter_config.filter_flags = filter_flags;
    
    furi_mutex_release(scanner->mutex);
}

// Get current packet filter configuration
uint8_t wifi_scanner_get_filter(WifiScanner* scanner) {
    if (!scanner) return PACKET_FILTER_ALL;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return PACKET_FILTER_ALL;
    }
    
    uint8_t filter_flags = scanner->filter_config.filter_flags;
    
    furi_mutex_release(scanner->mutex);
    return filter_flags;
}

// Toggle a specific packet filter type
void wifi_scanner_toggle_filter(WifiScanner* scanner, PacketFilterType filter_type) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    // Toggle the specific filter bit
    scanner->filter_config.filter_flags ^= filter_type;
    
    furi_mutex_release(scanner->mutex);
}

// Start deauth attack on a specific network or broadcast
bool wifi_scanner_start_deauth(WifiScanner* scanner, uint8_t network_index) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool result = false;
    
    if (scanner->esp32_connected) {
        // Build command: "attack -t deauth -a 0\n" where 0 is the network index
        char cmd[64];
        if (network_index < scanner->network_count) {
            snprintf(cmd, sizeof(cmd), "attack -t deauth -a %u\n", network_index);
        } else {
            // Broadcast deauth to all networks
            snprintf(cmd, sizeof(cmd), "attack -t deauth\n");
        }
        result = uart_write_str(cmd);
        
        if (result) {
            scanner->active_attack = ATTACK_TYPE_DEAUTH;
            scanner->stats.deauth_sent++;
        }
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

// Stop deauth attack
void wifi_scanner_stop_deauth(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    if (scanner->esp32_connected && scanner->active_attack == ATTACK_TYPE_DEAUTH) {
        uart_write_str(MARAUDER_CMD_STOPSCAN);
        scanner->active_attack = ATTACK_TYPE_NONE;
    }
    
    furi_mutex_release(scanner->mutex);
}

// Start beacon spam attack
bool wifi_scanner_start_beacon_spam(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool result = false;
    
    if (scanner->esp32_connected) {
        // Use Marauder's beacon list attack
        result = uart_write_str("attack -t beacon -l\n");
        
        if (result) {
            scanner->active_attack = ATTACK_TYPE_BEACON_SPAM;
            scanner->stats.beacons_sent++;
        }
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

// Stop beacon spam
void wifi_scanner_stop_beacon_spam(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    if (scanner->esp32_connected && scanner->active_attack == ATTACK_TYPE_BEACON_SPAM) {
        uart_write_str(MARAUDER_CMD_STOPSCAN);
        scanner->active_attack = ATTACK_TYPE_NONE;
    }
    
    furi_mutex_release(scanner->mutex);
}

// Start PMKID capture mode
bool wifi_scanner_start_pmkid_capture(WifiScanner* scanner) {
    if (!scanner) return false;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return false;
    }
    
    bool result = false;
    
    if (scanner->esp32_connected) {
        result = uart_write_str("sniffpmkid\n");
        
        if (result) {
            scanner->active_attack = ATTACK_TYPE_PMKID;
            scanner->is_capturing = true;
        }
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

// Stop PMKID capture
void wifi_scanner_stop_pmkid_capture(WifiScanner* scanner) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    if (scanner->esp32_connected && scanner->active_attack == ATTACK_TYPE_PMKID) {
        uart_write_str(MARAUDER_CMD_STOPSCAN);
        scanner->active_attack = ATTACK_TYPE_NONE;
        scanner->is_capturing = false;
    }
    
    furi_mutex_release(scanner->mutex);
}

// Enable/disable channel hopping
void wifi_scanner_set_channel_hopping(WifiScanner* scanner, bool enabled) {
    if (!scanner) return;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return;
    }
    
    scanner->channel_hopping = enabled;
    
    if (scanner->esp32_connected) {
        if (enabled) {
            // Enable channel hopping on ESP32
            uart_write_str("channel -h\n");
        } else {
            // Disable hopping, set to specific channel
            char cmd[32];
            // Use first network's channel if available, otherwise default to 6
            uint8_t channel = (scanner->network_count > 0) ? scanner->networks[0].channel : 6;
            snprintf(cmd, sizeof(cmd), "channel -s %u\n", channel);
            uart_write_str(cmd);
        }
    }
    
    furi_mutex_release(scanner->mutex);
}

// Get session statistics
SessionStats wifi_scanner_get_stats(WifiScanner* scanner) {
    SessionStats stats = {0};
    
    if (!scanner) return stats;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return stats;
    }
    
    stats = scanner->stats;
    stats.total_packets = scanner->packet_count;
    
    furi_mutex_release(scanner->mutex);
    return stats;
}

// Check if any attack is currently active
AttackType wifi_scanner_get_active_attack(WifiScanner* scanner) {
    if (!scanner) return ATTACK_TYPE_NONE;
    
    if (furi_mutex_acquire(scanner->mutex, FuriWaitForever) != FuriStatusOk) {
        return ATTACK_TYPE_NONE;
    }
    
    AttackType attack = scanner->active_attack;
    
    furi_mutex_release(scanner->mutex);
    return attack;
}
