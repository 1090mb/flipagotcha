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

#### New Commands (protocol.h)
- `CMD_SCAN_NETWORKS (0x07)` - Command to scan for WiFi networks
- `CMD_START_HANDSHAKE (0x08)` - Command to initiate handshake
- `CMD_CAPTURE_PACKET (0x09)` - Command to capture packets
- `CMD_SAVE_CAPTURE (0x0A)` - Command to save captured data

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

## ESP-32 Integration

The Flipagotcha app communicates with an ESP-32 via UART to perform actual WiFi operations. The ESP-32 should:

1. Listen for commands on UART (115200 baud)
2. Respond to scanning commands by switching WiFi channels
3. Capture and forward 802.11 packets via UART
4. Perform deauthentication when handshake is requested

### Expected ESP-32 Behavior

#### On CMD_SCAN_NETWORKS (0x07):
- Switch WiFi adapter to monitor mode
- Scan channels 1-14 for beacon frames
- Send discovered network information back via UART

#### On CMD_START_HANDSHAKE (0x08):
- Receive network index
- Send deauth packets to trigger handshake
- Capture EAPOL frames (4-way handshake)
- Forward captured frames via UART

#### On CMD_CAPTURE_PACKET (0x09):
- Start capturing all 802.11 frames
- Forward captured frames via UART

#### On CMD_SAVE_CAPTURE (0x0A):
- Flush any remaining buffered packets

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
- Packet size: Maximum 2048 bytes per packet (MAX_PACKET_SIZE)

### Mock Data
The current implementation includes mock data for demonstration purposes when an ESP-32 is not connected. This simulates:
- 3 WiFi networks with varying signal strengths
- Simulated handshake packet capture

For production use, these should be replaced with actual data from the ESP-32.
