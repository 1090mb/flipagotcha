# Bettercap-Inspired Features in Flipagotcha

This document details the bettercap-inspired attack capabilities and advanced features implemented in Flipagotcha, making it a powerful WiFi security testing tool for the Flipper Zero.

## Overview

Flipagotcha brings the power of bettercap's WiFi attack capabilities to the Flipper Zero platform, integrating with ESP32 Marauder firmware to perform sophisticated WiFi security testing operations.

## Attack Modes

### 1. Deauthentication Attack

**Description**: Send deauthentication frames to disconnect clients from access points.

**Use Cases**:
- Test network resilience against deauth attacks
- Force clients to reconnect (useful for handshake capture)
- Assess WiFi security implementation

**How to Use**:
1. Press **Left Arrow** from main screen to open Attack Menu
2. Select "Deauth Attack" with Up/Down arrows
3. Press **OK** to start
4. Select "Stop Attack" to end

**ESP32 Marauder Command**: `attack -t deauth -a <AP_INDEX>`

**Features**:
- Target specific network by index
- Broadcast deauth to all networks
- Real-time attack status display on main screen
- Tracks number of deauth frames sent in statistics

### 2. Beacon Spam Attack

**Description**: Inject fake beacon frames to create spoofed or fake WiFi networks.

**Use Cases**:
- Test SSID filtering and network detection systems
- Demonstrate WiFi spoofing capabilities
- Educational purposes for WiFi beacon frame structure

**How to Use**:
1. Open Attack Menu (Left Arrow)
2. Select "Beacon Spam"
3. Press **OK** to activate
4. Monitor "ATK: Beacon" status on main screen

**ESP32 Marauder Command**: `attack -t beacon -l`

**Features**:
- Creates multiple fake SSIDs
- Continuous beacon frame transmission
- Attack indicator on main screen

### 3. PMKID Capture Mode

**Description**: Specialized capture mode for obtaining PMKID hashes from WPA/WPA2 networks.

**Use Cases**:
- Capture PMKIDs for offline password cracking
- Test WPA2 security without client deauthentication
- More passive than traditional handshake capture

**How to Use**:
1. Scan for networks first (press OK)
2. Open Attack Menu
3. Select "PMKID Capture"
4. Press **OK** to start capturing

**ESP32 Marauder Command**: `sniffpmkid`

**Features**:
- Targets all networks in range
- Captured PMKIDs saved to storage
- Statistics track PMKID capture count
- Less intrusive than deauth attacks

### 4. Channel Hopping

**Description**: Automatically cycle through WiFi channels to maximize network discovery and packet capture.

**Use Cases**:
- Discover networks on all channels
- Comprehensive packet capture
- Better coverage during reconnaissance

**How to Use**:
1. Open Attack Menu
2. Navigate to "Channel Hop"
3. Press **OK** to toggle on/off
4. Checkbox indicates current state

**ESP32 Marauder Command**: `channel -h` (enable) or `channel -s <CH>` (set specific)

**Features**:
- Cycles through channels 1-14
- Toggle on/off independently of other attacks
- Can combine with scanning or attacks

## Menu System

### Attack Menu (Left Arrow)
```
> Deauth Attack
  Beacon Spam
  PMKID Capture
  Stop Attack
  Channel Hop [X]
  Back
```

Options:
- **Deauth Attack**: Start deauthentication attack
- **Beacon Spam**: Begin beacon frame injection
- **PMKID Capture**: Enable PMKID capture mode
- **Stop Attack**: Halt any active attack
- **Channel Hop**: Toggle automatic channel hopping
- **Back**: Return to main screen

### Statistics View (Right Arrow)
```
Statistics
Handshakes: 3
Deauths: 150
Beacons: 500
PMKIDs: 2
Packets: 1234
```

Displays:
- **Handshakes**: WPA/WPA2 handshakes captured
- **Deauths**: Deauth frames sent
- **Beacons**: Beacon frames injected
- **PMKIDs**: PMKID hashes captured
- **Packets**: Total packets captured

### Packet Filter Menu (Up Arrow)
```
Packet Filters
> Beacon     [X]
  Probe Req  [X]
  Probe Resp [X]
  Data       [X]
  Deauth     [X]
  EAPOL      [X]
  Back
```

Filter by packet type:
- **Beacon**: Beacon frames (network advertisements)
- **Probe Req**: Probe request frames (client searching)
- **Probe Resp**: Probe response frames (AP responses)
- **Data**: Data frames (actual network traffic)
- **Deauth**: Deauthentication frames
- **EAPOL**: EAPOL frames (handshake packets)

Each filter can be toggled independently to capture only desired packet types.

### Settings Menu (Down Arrow)
```
Settings
> Auto Save: OFF
  Save Interval: 5m
  Audio: OFF
  Channel: Auto
  Back
```

Configuration options:
- **Auto Save**: Automatic capture file saving
- **Save Interval**: How often to auto-save
- **Audio**: Sound feedback for captures
- **Channel**: Manual channel selection or auto

*(Some settings are placeholders for future implementation)*

## Dynamic Face Expressions

The face on screen changes to reflect the app's current state:

| Expression | State | Description |
|------------|-------|-------------|
| ^_^ | Happy | Packets successfully captured |
| >_< | Attacking | Active attack in progress |
| •_• | Scanning | Scanning for networks |
| x_x | Confused | ESP32 not connected (DEMO mode) |
| •‿• | Normal | Ready and idle |
| -_- | Sleeping | Scanner not initialized |

## ESP32 Marauder Integration

Flipagotcha communicates with ESP32 Marauder firmware via UART to execute attacks:

### Supported Commands
- `attack -t deauth -a <INDEX>` - Deauth attack on specific AP
- `attack -t beacon -l` - Beacon spam using list
- `sniffpmkid` - PMKID capture mode
- `channel -h` - Enable channel hopping
- `channel -s <CH>` - Set specific channel
- `scanap` - Scan for access points
- `stopscan` - Stop current operation

### Connection Status
- **ESP32** indicator: Real ESP32 connected and responsive
- **DEMO** indicator: No ESP32 detected, using mock data
- Status icon: Filled square (connected) or X'd square (disconnected)

## Session Statistics

Statistics are tracked throughout the session:

```c
typedef struct {
    uint32_t handshakes_captured;  // WPA handshakes captured
    uint32_t deauth_sent;           // Deauth frames transmitted
    uint32_t beacons_sent;          // Beacon frames transmitted
    uint32_t pmkids_captured;       // PMKID hashes captured
    uint32_t session_start_time;    // Session start timestamp
    uint32_t total_packets;         // Total packets captured
} SessionStats;
```

## Comparison with Pwnagotchi

### What Makes Flipagotcha Different

**Advantages**:
1. **Portable**: Flipper Zero is smaller and more portable than Raspberry Pi
2. **Interactive UI**: Real-time menu-driven interface vs. passive automation
3. **Manual Control**: Direct control over attacks vs. AI-driven automation
4. **Instant Feedback**: Immediate visual feedback with expressive face
5. **Multi-function**: Part of broader Flipper Zero toolkit

**Pwnagotchi Advantages**:
1. **AI Learning**: Adaptive behavior based on environment
2. **Mesh Networking**: Multiple units can cooperate
3. **More Power**: Raspberry Pi has more processing capability
4. **Longer Sessions**: Better battery life for extended operations
5. **Plugin Ecosystem**: Extensive community plugins

### Unique Flipagotcha Features

1. **Multi-Menu Navigation**: Quick access to attacks, filters, stats, and settings
2. **Real-time Attack Control**: Start/stop attacks with button presses
3. **Packet Filtering**: Granular control over which packet types to capture
4. **Expressive Face**: Visual feedback through different expressions
5. **Compact Form Factor**: Fits in pocket with Flipper Zero
6. **BadUSB Integration**: (Future) Combine with Flipper's BadUSB for automated cracking

## Best Practices

### Legal and Ethical Use
- Only use on networks you own or have explicit permission to test
- Follow local laws regarding WiFi security testing
- Document your testing authorization
- Understand the legal implications in your jurisdiction

### Effective Testing
1. **Start with Scanning**: Always scan for networks first (press OK)
2. **Use Filters**: Enable only needed packet types to save storage
3. **Monitor Statistics**: Check stats view to track progress
4. **Save Frequently**: Stop scanning periodically to save captures
5. **Channel Hopping**: Enable for comprehensive network discovery

### Maximizing Captures
1. **Position**: Get physically close to target networks
2. **Duration**: Run attacks for several minutes for best results
3. **PMKID First**: Try PMKID capture before deauth (less intrusive)
4. **Deauth Last**: Use deauth attacks only when needed
5. **Save Data**: Remember to save captures before exiting

## Troubleshooting

### No ESP32 Connection
- Verify UART connections (TX, RX, GND)
- Check ESP32 is powered on
- Ensure Marauder firmware is installed
- App shows "DEMO" when ESP32 not detected

### No Attacks Working
- Confirm ESP32 connection (not in DEMO mode)
- Verify Marauder firmware supports attack commands
- Check ESP32 has WiFi antenna connected
- Ensure ESP32 is not in a mode that blocks attacks

### Low Capture Rate
- Enable channel hopping for better coverage
- Move closer to target networks
- Extend attack duration
- Check packet filters aren't too restrictive

## Future Enhancements

Planned features:
- Audio feedback for successful captures
- Automatic capture saving at intervals
- GPS integration for wardrive mode
- Enhanced statistics and graphing
- BadUSB integration for automated cracking
- Custom attack scripts
- Network quality visualization
- Improved PMKID parsing and display

## References

- [Bettercap Documentation](https://www.bettercap.org/)
- [ESP32 Marauder](https://github.com/justcallmekoko/ESP32Marauder)
- [Pwnagotchi](https://pwnagotchi.ai/)
- [Flipper Zero Official Docs](https://docs.flipperzero.one/)

## Conclusion

Flipagotcha brings professional-grade WiFi security testing capabilities to the Flipper Zero, combining the power of bettercap-style attacks with an intuitive, portable interface. Whether you're a security professional, researcher, or enthusiast, Flipagotcha provides the tools you need for comprehensive WiFi security assessment.

**Remember**: Always use responsibly and legally!
