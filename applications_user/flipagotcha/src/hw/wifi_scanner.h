#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../common/protocol.h"

typedef struct WifiScanner WifiScanner;

// Initialize the WiFi scanner
WifiScanner* wifi_scanner_alloc(void);

// Free the WiFi scanner
void wifi_scanner_free(WifiScanner* scanner);

// Start scanning for WiFi networks
bool wifi_scanner_start_scan(WifiScanner* scanner);

// Stop scanning
void wifi_scanner_stop_scan(WifiScanner* scanner);

// Get the number of networks found
uint8_t wifi_scanner_get_network_count(WifiScanner* scanner);

// Get a network by index
const WifiNetwork* wifi_scanner_get_network(WifiScanner* scanner, uint8_t index);

// Initiate handshake with a specific network
bool wifi_scanner_start_handshake(WifiScanner* scanner, uint8_t network_index);

// Start capturing packets on current channel
bool wifi_scanner_start_capture(WifiScanner* scanner);

// Stop capturing packets
void wifi_scanner_stop_capture(WifiScanner* scanner);

// Get the number of packets captured
uint16_t wifi_scanner_get_packet_count(WifiScanner* scanner);

// Save captured packets to storage
bool wifi_scanner_save_capture(WifiScanner* scanner, const char* filename);

// Check if scanner is currently active
bool wifi_scanner_is_scanning(WifiScanner* scanner);

// Check if capture is active
bool wifi_scanner_is_capturing(WifiScanner* scanner);

// Check if ESP32 is connected
bool wifi_scanner_is_esp32_connected(WifiScanner* scanner);

// Refresh/check the ESP32 connection state (queries UART) and update internal flag
// Returns the new connection state
bool wifi_scanner_refresh_connection(WifiScanner* scanner);

// Set packet filter configuration
void wifi_scanner_set_filter(WifiScanner* scanner, uint8_t filter_flags);

// Get current packet filter configuration
uint8_t wifi_scanner_get_filter(WifiScanner* scanner);

// Toggle a specific packet filter type
void wifi_scanner_toggle_filter(WifiScanner* scanner, PacketFilterType filter_type);

// Start deauth attack on a specific network or broadcast
bool wifi_scanner_start_deauth(WifiScanner* scanner, uint8_t network_index);

// Stop deauth attack
void wifi_scanner_stop_deauth(WifiScanner* scanner);

// Start beacon spam attack
bool wifi_scanner_start_beacon_spam(WifiScanner* scanner);

// Stop beacon spam
void wifi_scanner_stop_beacon_spam(WifiScanner* scanner);

// Start PMKID capture mode
bool wifi_scanner_start_pmkid_capture(WifiScanner* scanner);

// Stop PMKID capture
void wifi_scanner_stop_pmkid_capture(WifiScanner* scanner);

// Enable/disable channel hopping
void wifi_scanner_set_channel_hopping(WifiScanner* scanner, bool enabled);

// Get session statistics
SessionStats wifi_scanner_get_stats(WifiScanner* scanner);

// Check if any attack is currently active
AttackType wifi_scanner_get_active_attack(WifiScanner* scanner);
