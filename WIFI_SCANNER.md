# WiFi Scanner Feature Documentation

## Overview
The WiFi scanner feature enables the Flipagotcha app to:
- Scan for available WiFi networks
- Initiate WPA/WPA2 handshakes with detected networks
- Capture WiFi packets for analysis
- Save captured packets to storage for later analysis

## Architecture

### New Modules

#### 1. wifi_scanner.c / wifi_scanner.h
Core WiFi scanning module that provides:
- `wifi_scanner_alloc()` - Allocate scanner instance
- `wifi_scanner_free()` - Free scanner resources
- `wifi_scanner_start_scan()` - Start WiFi network scanning
- `wifi_scanner_stop_scan()` - Stop scanning
- `wifi_scanner_get_network_count()` - Get number of networks found
- `wifi_scanner_get_network()` - Get network details by index
- `wifi_scanner_start_handshake()` - Initiate handshake with a network
- `wifi_scanner_start_capture()` - Start packet capture
- `wifi_scanner_stop_capture()` - Stop packet capture
- `wifi_scanner_get_packet_count()` - Get number of packets captured
- `wifi_scanner_save_capture()` - Save captured packets to file

### Protocol Extensions

#### Legacy Binary Commands (protocol.h)
These are retained for backward compatibility but the app now uses text-based Marauder CLI commands:
- `CMD_SCAN_NETWORKS (0x07)` - Legacy scan command
- `CMD_START_HANDSHAKE (0x08)` - Legacy handshake command
- `CMD_CAPTURE_PACKET (0x09)` - Legacy capture command
- `CMD_SAVE_CAPTURE (0x0A)` - Legacy save command

#### Marauder Text Commands (Actual Implementation)
The app now uses Marauder's native CLI commands:
- `scanap` - Scan for access points
- `sniffpmkid -c <channel> -d` - Capture handshake with deauth
- `sniffraw` - Capture raw packets
- `stopscan` - Stop operations

#### New Data Structures
```c
typedef struct {
    char ssid[MAX_SSID_LEN + 1];    // Network name
    uint8_t bssid[6];                // MAC address
    int8_t rssi;                     // Signal strength
    uint8_t channel;                 // WiFi channel
    uint8_t encryption;              // Encryption type
} WifiNetwork;

typedef struct {
    uint8_t data[MAX_PACKET_SIZE];   // Packet data
    uint16_t length;                 // Packet length
    uint32_t timestamp;              // Capture timestamp
    uint8_t channel;                 // Channel number
} CapturedPacket;
```

## User Interface

### Button Controls
- **OK button**: Toggle WiFi scanning on/off
  - First press: Start scan and packet capture
  - Second press: Stop scan and save captured packets
- **Left arrow**: Initiate handshake with first detected network
- **Right arrow**: Open eyes (unchanged)
- **Up arrow**: Change to smile (unchanged)
- **Down arrow**: Change to frown (unchanged)
- **Back button**: Exit app (unchanged)

### Display Information
- Top left: Scanning status and network count
- Bottom left: Captured packet count
- Center: Animated face (unchanged)

## Data Storage

Captured packets are saved to:
```
/ext/apps_data/flipagotcha/capture.txt
```

### File Format
```
# Flipagotcha Packet Capture
# Packets: <count>

Packet 0: len=<length>, ts=<timestamp>, ch=<channel>
<hex dump of packet data>

Packet 1: len=<length>, ts=<timestamp>, ch=<channel>
<hex dump of packet data>
...
```

## ESP-32 Marauder Integration

The Flipagotcha app communicates with an ESP-32 running Marauder firmware via UART (115200 baud, 8N1) to perform WiFi operations.

### Marauder CLI Commands Used

The app sends these text-based commands to the ESP32:

#### Network Scanning
- **scanap** - Scans for WiFi access points and returns results in format:
  ```
  [CH 06] NetworkName (-45dBm) [WPA2]
  ```

#### Packet Capture  
- **sniffpmkid -c <channel> -d** - Captures PMKID/EAPOL handshake frames
  - `-c <channel>` - Specifies WiFi channel to monitor
  - `-d` - Enables deauthentication to force handshake
  
- **sniffraw** - Captures all raw 802.11 WiFi frames

#### Control Commands
- **stopscan** - Stops any active scanning or sniffing operation

### Response Parsing

The app parses Marauder's text output to extract:
- **SSID**: Network name
- **BSSID**: Access point MAC address (if provided)
- **RSSI**: Signal strength in dBm
- **Channel**: WiFi channel number (1-14)
- **Encryption**: Security type (OPEN, WPA, WPA2, WPA3)

### Connection Detection

On initialization, the app:
1. Sends a test command to ESP32
2. Waits for response
3. Sets connection status (displayed as "ESP32" or "DEMO" in UI)

### Fallback Behavior

If ESP32 is not connected:
- App operates in DEMO mode with mock data
- All UI features remain functional
- Useful for testing without hardware

### Hardware Connection

Connect ESP32 to Flipper Zero GPIO pins:
- TX (Pin 13) → ESP32 RX
- RX (Pin 14) → ESP32 TX  
- GND (Pin 8) → ESP32 GND

See [ESP32_SETUP.md](ESP32_SETUP.md) for complete setup instructions.

## Building

Follow the standard Flipper Zero build process:
```bash
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha
```

## Security Considerations

This tool is designed for:
- Educational purposes
- Security research on networks you own
- Penetration testing with proper authorization

**WARNING**: Unauthorized WiFi scanning, deauthentication attacks, or packet capture may be illegal in your jurisdiction. Always ensure you have permission before using this tool on any network.

## Implementation Notes

### Thread Safety
The wifi_scanner module uses FuriMutex to ensure thread-safe access to shared data structures.

### Memory Management
- Networks: Limited to 16 simultaneous entries (MAX_NETWORKS)
- Packets: Dynamic allocation starting at 100 packets, expandable
- Packet size: Maximum 512 bytes per packet (MAX_PACKET_SIZE) - reduced to save Flipper RAM

### Demo Mode Fallback
The implementation includes fallback to mock data when an ESP32 is not connected. This simulates:
- 3 WiFi networks with varying signal strengths
- Simulated handshake packet capture

This allows testing the app interface without ESP32 hardware.
