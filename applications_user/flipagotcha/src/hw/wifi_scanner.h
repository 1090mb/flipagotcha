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
