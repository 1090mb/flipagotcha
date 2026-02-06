# Features

Comprehensive documentation of all Flipagotcha features and capabilities.

## Table of Contents

- [Overview](#overview)
- [Animated Face](#animated-face)
- [WiFi Network Scanning](#wifi-network-scanning)
- [WPA Handshake Capture](#wpa-handshake-capture)
- [Packet Capture](#packet-capture)
- [Data Storage](#data-storage)
- [Demo Mode](#demo-mode)
- [UART Communication](#uart-communication)
- [Thread Safety](#thread-safety)

## Overview

Flipagotcha combines a Pwnagotchi-style animated interface with serious WiFi security research capabilities powered by ESP32 Marauder firmware.

### Feature Matrix

| Feature | Demo Mode | With ESP32 | Notes |
|---------|-----------|------------|-------|
| Animated Face | ✅ | ✅ | Always available |
| Face Expressions | ✅ | ✅ | Interactive controls |
| WiFi Scanning | ✅ (Simulated) | ✅ (Real) | Auto-detects mode |
| Network Detection | ✅ (3 networks) | ✅ (Unlimited) | Real-time updates |
| Handshake Capture | ✅ (Simulated) | ✅ (Real) | WPA/WPA2/WPA3 |
| Packet Capture | ✅ (Mock data) | ✅ (Real 802.11) | Raw packets |
| Data Storage | ✅ | ✅ | SD card storage |
| Channel Monitoring | ❌ | ✅ | Channels 1-14 |
| Deauth Attacks | ❌ | ✅ | Via Marauder |

## Animated Face

### Description

A minimalist Pwnagotchi-style animated face drawn with Canvas primitives.

### Features

- **Blinking Eyes**: Automatic eye blink animation
- **Expressions**: Smile and frown modes
- **Manual Control**: Open/close eyes on demand
- **Smooth Animation**: 60 FPS rendering
- **Low Resource**: No PNG assets, purely programmatic

### Implementation Details

- Drawn using Flipper Zero Canvas API
- No external image assets needed
- Lightweight binary size
- Rendered in real-time

### Customization

Face drawing code is in `src/ui/draw.c`:
- Modify eye shapes
- Add new expressions
- Change face geometry
- Customize animation timing

## WiFi Network Scanning

### Description

Real-time detection and enumeration of WiFi networks in the 2.4GHz band.

### Capabilities

- **Channel Range**: Scans WiFi channels 1-14
- **Network Information**: 
  - SSID (network name)
  - BSSID (MAC address)
  - Channel number
  - RSSI (signal strength)
  - Encryption type (OPEN, WPA, WPA2, WPA3)
- **Real-time Updates**: Network count updates during scan
- **Automatic Discovery**: Finds hidden and visible networks

### How It Works

#### With ESP32 Marauder

1. **Command Sent**: `scanap`
2. **Channel Hopping**: ESP32 cycles through channels 1-14
3. **Beacon Detection**: Captures beacon frames from APs
4. **Response Parsing**: Extracts network information
5. **Display Update**: Network count shown on screen

#### Response Format

Marauder outputs networks as:
```
[CH 06] NetworkName (-45dBm) [WPA2]
```

Flipagotcha parses:
- Channel: `06`
- SSID: `NetworkName`
- RSSI: `-45` dBm
- Encryption: `WPA2`

### Supported Network Types

- **Open Networks**: No encryption
- **WEP**: Legacy encryption (deprecated)
- **WPA**: WiFi Protected Access
- **WPA2**: WPA version 2 (most common)
- **WPA3**: Latest WPA version

### Limitations

- **2.4GHz Only**: ESP32 doesn't support 5GHz
- **802.11 b/g/n**: Modern standards
- **Maximum Range**: Depends on ESP32 antenna
- **Scan Time**: ~30-60 seconds for complete scan

## WPA Handshake Capture

### Description

Captures the WPA/WPA2 4-way handshake used for WiFi authentication.

### What is a Handshake?

The WPA handshake is a 4-step authentication process:

1. **ANonce**: AP sends authentication nonce to client
2. **SNonce**: Client responds with its nonce + MIC
3. **GTK**: AP sends group temporal key
4. **ACK**: Client acknowledges receipt

### Capture Process

#### Initiation

1. User presses Left button
2. App selects first detected network
3. Sends command: `sniffpmkid -c <channel> -d`

#### Deauthentication

ESP32 Marauder:
- Sends deauthentication frames to clients
- Forces clients to disconnect
- Triggers reconnection and handshake

#### Monitoring

- Locks to target network's channel
- Captures all EAPOL frames
- Stores handshake packets
- Continues until stopped

### PMKID vs Full Handshake

#### PMKID (Pairwise Master Key Identifier)
- **Method**: Extracted from first handshake frame
- **Advantage**: Only needs one frame
- **Disadvantage**: Not all APs support it

#### Full 4-Way Handshake
- **Method**: Captures all 4 frames
- **Advantage**: Works with all WPA/WPA2 APs
- **Disadvantage**: Needs active client

Marauder's `sniffpmkid` captures both!

### Use Cases

- **Password Testing**: Verify network password strength
- **Security Audit**: Test WPA implementation
- **Penetration Testing**: Authorized security assessment
- **Network Analysis**: Study authentication patterns

### Output

Captured handshakes are saved in the packet capture file:
- EAPOL frame data
- Timestamp
- Channel information
- Can be converted to hashcat format

## Packet Capture

### Description

Raw 802.11 WiFi packet capture for detailed network analysis.

### Captured Frame Types

#### Management Frames
- **Beacon**: AP advertisements
- **Probe Request/Response**: Client scanning
- **Authentication**: Connection initiation
- **Association**: Connection establishment
- **Deauthentication**: Disconnection
- **Disassociation**: Connection termination

#### Data Frames
- **Data**: Encrypted user traffic
- **Null Data**: Power save, ACK
- **QoS Data**: Quality of Service traffic

#### Control Frames
- **RTS/CTS**: Request/Clear to Send
- **ACK**: Acknowledgments
- **Block ACK**: Aggregated ACK

### Packet Structure

Each captured packet includes:

```c
typedef struct {
    uint8_t data[MAX_PACKET_SIZE];  // Raw packet data (512 bytes max)
    uint16_t length;                // Actual packet length
    uint32_t timestamp;             // Capture time (ms)
    uint8_t channel;                // WiFi channel
} CapturedPacket;
```

### Memory Management

- **Initial Capacity**: 100 packets
- **Packet Size**: Maximum 512 bytes each
- **Total RAM**: ~50KB for packet buffer
- **Dynamic Growth**: Can expand if needed

### Storage Format

Packets saved as hex dump:

```
Packet 0: len=64, ts=12345678, ch=6
00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ................
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  ................
```

### Analysis Tools

Captured data can be analyzed with:
- **Wireshark**: Convert to PCAP format
- **tcpdump**: Command-line analysis
- **aircrack-ng**: WiFi security tools
- **Custom scripts**: Python/Bash parsing

## Data Storage

### File System

Flipagotcha uses Flipper Zero's storage system:

```
/ext/                           # SD card root
└── apps_data/                  # App data directory
    └── flipagotcha/            # Flipagotcha directory
        └── capture.txt         # Packet capture file
```

### Directory Creation

Created automatically on first launch:
- `/ext/apps_data/flipagotcha/`
- Permissions: Read/write
- Persistent across reboots

### Capture File Format

#### Header
```
# Flipagotcha Packet Capture
# Packets: <count>
```

#### Packet Entries
```
Packet <N>: len=<bytes>, ts=<milliseconds>, ch=<channel>
<hex dump of packet data>
```

### File Management

- **Append Mode**: Each scan adds to file
- **No Automatic Cleanup**: User must delete old files
- **File Size**: Can grow large with many scans
- **SD Card**: Limited by card capacity

### Export Options

1. **qFlipper**: Download via USB
2. **SD Card Reader**: Direct file access
3. **Flipper File Manager**: On-device management

## Demo Mode

### Purpose

Allows app testing without ESP32 hardware.

### Activation

Demo mode activates when:
- No ESP32 detected on GPIO
- ESP32 doesn't respond to commands
- Marauder firmware not found

### Simulated Features

#### Mock Networks
```
Network 1:
  SSID: "DemoNet-2.4G"
  Channel: 6
  RSSI: -45 dBm
  Encryption: WPA2

Network 2:
  SSID: "TestRouter"
  Channel: 11
  RSSI: -67 dBm
  Encryption: WPA

Network 3:
  SSID: "OpenWiFi"
  Channel: 1
  RSSI: -82 dBm
  Encryption: OPEN
```

#### Mock Packets
- Simulated beacon frames
- Fake EAPOL handshake
- Random data packets
- Realistic packet sizes

### Behavior

- All buttons work normally
- Network scanning shows mock data
- Handshake capture simulated
- Packet count increases
- Files saved with mock data

### Use Cases

- **Testing**: Verify app functionality
- **Development**: Debug without hardware
- **Demonstration**: Show app to others
- **Tutorial**: Learn interface before real use

## UART Communication

### Protocol

- **Baud Rate**: 115200
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Configuration**: 8N1

### Pin Configuration

| Function | Flipper GPIO | ESP32 GPIO |
|----------|--------------|------------|
| TX       | Pin 13       | RX (varies) |
| RX       | Pin 14       | TX (varies) |
| GND      | Pin 8        | GND |

### Command Protocol

Flipagotcha sends text commands to Marauder:

```
scanap\n
sniffpmkid -c 6 -d\n
sniffraw\n
stopscan\n
```

### Response Parsing

Marauder responses are parsed line-by-line:
- Extract network information
- Parse packet data
- Update internal state
- Refresh display

### Connection Detection

1. Send `help\n` command
2. Wait for response
3. Parse output for Marauder signature
4. Set connection status

### Error Handling

- **Timeout**: 5-second wait for responses
- **Retry**: Not implemented (fail gracefully)
- **Fallback**: Switch to demo mode on failure

## Thread Safety

### Design

All shared data protected by FuriMutex.

### Data Structures

Protected structures:
- Network list
- Packet buffer
- Scanner state
- Counters

### Mutex Pattern

```c
// Acquire mutex
furi_mutex_acquire(scanner->mutex, FuriWaitForever);

// Access shared data
// ... read/write operations ...

// Release mutex
furi_mutex_release(scanner->mutex);
```

### Worker Thread

UART communication uses background worker:
- Processes incoming data asynchronously
- Doesn't block main UI thread
- Callback-based architecture
- Thread-safe buffer access

### Race Condition Prevention

Critical sections protected:
- Network count checks
- Packet buffer access
- Scanner state changes
- UART buffer operations

## Security Features

### Input Validation

All untrusted input validated:
- **Channel**: Range 1-14
- **RSSI**: Range -100 to 0 dBm
- **String lengths**: Buffer overflow prevention
- **Packet sizes**: Maximum size enforcement

### Buffer Protection

- Bounds checking before writes
- Safe string operations (strncpy, snprintf)
- Fixed-size buffers
- Overflow detection

### Command Injection Prevention

- Channel numbers validated before use
- No user input directly in commands
- Sanitized UART communication

## Performance

### Resource Usage

- **RAM**: ~50KB for packet buffer
- **CPU**: Minimal, mostly idle
- **Battery**: ~2-3 hours continuous use
- **SD Card**: Depends on capture size

### Optimization

- Efficient Canvas rendering
- Minimal allocations
- Conservative buffer sizes
- Low-power idle state

## Future Enhancements

Potential future features:
- PCAP file export
- Handshake verification
- Network selection UI
- Channel hopping control
- Extended packet filters
- Real-time packet analysis
- GPS coordinates (with GPS module)
- Bluetooth scanning (ESP32 capable)

## Limitations

### Hardware
- 2.4GHz WiFi only (no 5GHz)
- Limited by ESP32 range
- Flipper Zero battery capacity

### Software
- Packet buffer size limited
- No real-time packet parsing
- Basic file format (not PCAP)
- Single network handshake target

### Legal
- Educational use only
- Requires authorization
- May be illegal without permission

See [Legal and Safety](Legal-and-Safety.md) for important information.

---

**Next**: [Architecture](Architecture.md) | [API Reference](API-Reference.md) | [User Guide](User-Guide.md)
