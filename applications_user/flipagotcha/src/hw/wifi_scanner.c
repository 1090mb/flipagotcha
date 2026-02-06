#include "wifi_scanner.h"
#include "uart.h"
#include <furi.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <string.h>

struct WifiScanner {
    WifiNetwork networks[MAX_NETWORKS];
    uint8_t network_count;
    CapturedPacket* packets;
    uint16_t packet_count;
    uint16_t packet_capacity;
    bool is_scanning;
    bool is_capturing;
    FuriMutex* mutex;
};

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
    free(scanner);
}

bool wifi_scanner_start_scan(WifiScanner* scanner) {
    if (!scanner) return false;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    // Reset network list
    scanner->network_count = 0;
    memset(scanner->networks, 0, sizeof(scanner->networks));
    
    // Send scan command to ESP32 via UART
    uint8_t cmd = CMD_SCAN_NETWORKS;
    bool result = uart_write(&cmd, 1);
    
    if (result) {
        scanner->is_scanning = true;
        
        // Simulate discovering some networks (in production, this would read from UART)
        // For demonstration purposes, we'll add some mock data
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
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

void wifi_scanner_stop_scan(WifiScanner* scanner) {
    if (!scanner) return;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    // Send stop command
    uint8_t cmd = CMD_STOP;
    uart_write(&cmd, 1);
    
    scanner->is_scanning = false;
    
    furi_mutex_release(scanner->mutex);
}

uint8_t wifi_scanner_get_network_count(WifiScanner* scanner) {
    if (!scanner) return 0;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    uint8_t count = scanner->network_count;
    furi_mutex_release(scanner->mutex);
    
    return count;
}

const WifiNetwork* wifi_scanner_get_network(WifiScanner* scanner, uint8_t index) {
    if (!scanner) return NULL;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    if (index >= scanner->network_count) {
        furi_mutex_release(scanner->mutex);
        return NULL;
    }
    
    const WifiNetwork* network = &scanner->networks[index];
    
    furi_mutex_release(scanner->mutex);
    return network;
}

bool wifi_scanner_start_handshake(WifiScanner* scanner, uint8_t network_index) {
    if (!scanner || network_index >= scanner->network_count) return false;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    // Send handshake command with network index
    uint8_t cmd[2] = {CMD_START_HANDSHAKE, network_index};
    bool result = uart_write(cmd, 2);
    
    // In production, this would initiate a deauth attack and capture handshake
    // For now, we simulate by capturing some packets
    if (result) {
        // Simulate handshake packet capture
        if (scanner->packet_count < scanner->packet_capacity) {
            CapturedPacket* pkt = &scanner->packets[scanner->packet_count];
            pkt->length = MOCK_HANDSHAKE_SIZE;
            pkt->timestamp = furi_get_tick();
            pkt->channel = scanner->networks[network_index].channel;
            memset(pkt->data, 0xAA, pkt->length); // Mock data
            scanner->packet_count++;
        }
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

bool wifi_scanner_start_capture(WifiScanner* scanner) {
    if (!scanner) return false;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    // Send capture command
    uint8_t cmd = CMD_CAPTURE_PACKET;
    bool result = uart_write(&cmd, 1);
    
    if (result) {
        scanner->is_capturing = true;
    }
    
    furi_mutex_release(scanner->mutex);
    return result;
}

void wifi_scanner_stop_capture(WifiScanner* scanner) {
    if (!scanner) return;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    uint8_t cmd = CMD_STOP;
    uart_write(&cmd, 1);
    
    scanner->is_capturing = false;
    
    furi_mutex_release(scanner->mutex);
}

uint16_t wifi_scanner_get_packet_count(WifiScanner* scanner) {
    if (!scanner) return 0;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    uint16_t count = scanner->packet_count;
    furi_mutex_release(scanner->mutex);
    
    return count;
}

bool wifi_scanner_save_capture(WifiScanner* scanner, const char* filename) {
    if (!scanner || !filename) return false;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    
    // Send save command
    uint8_t cmd = CMD_SAVE_CAPTURE;
    uart_write(&cmd, 1);
    
    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    
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
                    offset += snprintf(hex_line + offset, sizeof(hex_line) - offset,
                                     "%02X ", pkt->data[j + k]);
                }
                hex_line[offset++] = '\n';
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
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    bool scanning = scanner->is_scanning;
    furi_mutex_release(scanner->mutex);
    
    return scanning;
}

bool wifi_scanner_is_capturing(WifiScanner* scanner) {
    if (!scanner) return false;
    
    furi_mutex_acquire(scanner->mutex, FuriWaitForever);
    bool capturing = scanner->is_capturing;
    furi_mutex_release(scanner->mutex);
    
    return capturing;
}
