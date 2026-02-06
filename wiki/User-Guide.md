# User Guide

Complete guide to using Flipagotcha on your Flipper Zero.

## Table of Contents

- [Interface Overview](#interface-overview)
- [Button Controls](#button-controls)
- [Using the App](#using-the-app)
- [WiFi Scanning](#wifi-scanning)
- [Handshake Capture](#handshake-capture)
- [Packet Capture](#packet-capture)
- [Saved Data](#saved-data)
- [Demo Mode](#demo-mode)
- [Best Practices](#best-practices)

## Interface Overview

When you launch Flipagotcha, you'll see:

### Screen Layout

```
┌────────────────────────────┐
│ Networks: 5    ESP32/DEMO  │  ← Status bar
│                            │
│         ( ◉  ◉ )          │  ← Animated face
│           \__/             │     (eyes blink)
│                            │     (smile/frown)
│                            │
│ Packets: 42                │  ← Packet counter
└────────────────────────────┘
```

### Display Elements

1. **Top-Left Corner**
   - Scanning status
   - Network count (when scanning)

2. **Top-Right Corner**
   - **"ESP32"** - Real WiFi scanning via ESP32
   - **"DEMO"** - Demo mode with simulated data

3. **Center**
   - Animated face with expressions
   - Eyes blink automatically
   - Smile or frown based on mood

4. **Bottom-Left Corner**
   - Captured packet count
   - Updates during scanning

## Button Controls

### Primary Controls

| Button | Action | Description |
|--------|--------|-------------|
| **OK** | Toggle Scanning | Start/stop WiFi scan and packet capture |
| **Left** | Capture Handshake | Initiate handshake with first detected network |
| **Right** | Open Eyes | Manually open eyes |
| **Up** | Smile | Change face to smile expression |
| **Down** | Frown | Change face to frown expression |
| **Back** | Exit | Close app and return to menu |

### Button Behavior Details

#### OK Button - Toggle Scanning
- **First Press**: Starts WiFi scanning
  - Sends `scanap` command to ESP32
  - Begins packet capture
  - Network count updates in real-time
  
- **Second Press**: Stops scanning
  - Sends `stopscan` command
  - Saves captured packets to file
  - Returns to idle state

#### Left Button - Capture Handshake
- **Function**: Initiates WPA/WPA2 handshake capture
- **Target**: First detected network in list
- **Process**:
  1. Sends `sniffpmkid -c <channel> -d` command
  2. Monitors target network's channel
  3. Sends deauthentication frames
  4. Captures EAPOL handshake packets
- **Status**: Watch packet count increase

#### Right/Up/Down - Face Controls
- **Right**: Opens eyes (cosmetic)
- **Up**: Changes to smile expression
- **Down**: Changes to frown expression
- These work anytime, even during scanning

## Using the App

### First Launch

1. **Navigate to the app**
   - Applications → GPIO → Flipagotcha

2. **Launch**
   - Press OK to start

3. **Check connection status**
   - Look at top-right corner
   - "ESP32" = Hardware connected
   - "DEMO" = Running in demo mode

### Basic Workflow

```
Start App
   ↓
Check Status (ESP32 or DEMO)
   ↓
Press OK to Start Scan
   ↓
Wait for Networks to Appear
   ↓
(Optional) Press Left for Handshake
   ↓
Press OK to Stop & Save
   ↓
Exit with Back Button
```

## WiFi Scanning

### Starting a Scan

1. **Press OK button**
2. **Watch the display**
   - Network count appears top-left
   - Counter increases as networks found
   - Packet capture starts automatically

### What's Happening

When scanning:
- ESP32 sends `scanap` command to Marauder
- Marauder scans all 2.4GHz WiFi channels (1-14)
- Detected networks are parsed and counted
- Raw packets are captured simultaneously

### Scan Results

Networks are detected with:
- **SSID** - Network name
- **Channel** - WiFi channel (1-14)
- **RSSI** - Signal strength in dBm
- **Encryption** - WPA, WPA2, WPA3, or OPEN

### Stopping the Scan

1. **Press OK button again**
2. **Scan stops**
   - Network list cleared
   - Packets saved to file
   - Ready for next scan

### Demo Mode Scanning

Without ESP32, demo mode simulates:
- 3 WiFi networks
- Varying signal strengths
- Different encryption types
- Realistic behavior for testing

## Handshake Capture

### What is a Handshake?

A WPA/WPA2 handshake is a 4-way authentication exchange between:
- WiFi access point
- Connected client device

Capturing this exchange can be used for:
- Network security testing
- Password strength verification
- Penetration testing (authorized only)

### How to Capture

1. **Start WiFi scanning** (Press OK)
2. **Wait for networks** to be detected
3. **Press Left button** to capture handshake
4. **Monitor packet count** - should increase
5. **Wait 10-30 seconds** for capture
6. **Stop scanning** (Press OK)

### The Process

When you press Left:

1. **Target Selection**
   - Targets the first detected network
   - Uses the network's channel

2. **Channel Lock**
   - ESP32 locks to target channel
   - Monitors for EAPOL frames

3. **Deauthentication**
   - Sends deauth frames to clients
   - Forces reconnection
   - Triggers handshake exchange

4. **Packet Capture**
   - Captures EAPOL handshake packets
   - Saves to capture file
   - Can be analyzed later

### Success Indicators

- Packet count increases (20-50+ packets typical)
- No error messages
- Capture file contains EAPOL frames

### Legal Warning

⚠️ **Only capture handshakes from networks you own or have explicit permission to test!**

Deauthentication attacks may be illegal in your jurisdiction.

## Packet Capture

### Automatic Capture

When scanning starts:
- All WiFi packets are captured
- Stored in memory initially
- Saved to file when scanning stops

### What's Captured

- Beacon frames (network advertisements)
- Probe requests/responses
- Data frames
- Management frames
- Control frames
- EAPOL handshake packets

### Capture Limits

- **Memory limit**: ~100 packets in RAM
- **Packet size**: Maximum 512 bytes each
- **Total storage**: Limited by SD card space

### Manual Capture Control

Capture happens automatically:
- Starts with scanning (OK button)
- Stops when scanning stops (OK again)

## Saved Data

### File Location

Captured packets are saved to:
```
/ext/apps_data/flipagotcha/capture.txt
```

### File Format

```
# Flipagotcha Packet Capture
# Packets: 42

Packet 0: len=64, ts=12345678, ch=6
00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ................
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  ................
...

Packet 1: len=128, ts=12345789, ch=11
...
```

### Field Descriptions

- **len** - Packet length in bytes
- **ts** - Timestamp (milliseconds since start)
- **ch** - WiFi channel number
- **Hex dump** - Raw packet data in hexadecimal

### Accessing Capture Files

#### Method 1: Using qFlipper

1. Connect Flipper Zero to computer
2. Open qFlipper → File Manager
3. Navigate to `/ext/apps_data/flipagotcha/`
4. Download `capture.txt`

#### Method 2: SD Card Reader

1. Remove SD card from Flipper Zero
2. Insert into computer
3. Navigate to `/ext/apps_data/flipagotcha/`
4. Copy `capture.txt`

### Analyzing Captures

Captured data can be:
1. Viewed in text editor
2. Converted to PCAP format for Wireshark
3. Parsed with custom scripts
4. Analyzed for handshakes

## Demo Mode

### What is Demo Mode?

Demo mode runs when:
- No ESP32 is connected
- ESP32 doesn't respond
- Marauder firmware not detected

### Features in Demo Mode

- Simulated WiFi networks (3 networks)
- Varying signal strengths
- Different encryption types
- Simulated packet capture
- All buttons work normally
- Face animation unchanged

### Demo Networks

```
Network 1: "DemoNet-2.4G" - Channel 6, -45dBm, WPA2
Network 2: "TestRouter" - Channel 11, -67dBm, WPA
Network 3: "OpenWiFi" - Channel 1, -82dBm, OPEN
```

### Using Demo Mode

Perfect for:
- Testing app interface
- Learning button controls
- Demonstrating to others
- Development without hardware
- Troubleshooting issues

### Switching to Real Mode

Connect ESP32 with Marauder:
1. Exit the app (Back button)
2. Connect ESP32 to GPIO
3. Relaunch app
4. Should show "ESP32" instead of "DEMO"

## Best Practices

### For WiFi Scanning

1. **Start Clean**
   - Exit and restart app between sessions
   - Clears previous scan data

2. **Optimal Location**
   - Move to area with known networks
   - Avoid interference (microwaves, etc.)
   - 2.4GHz networks only

3. **Power Management**
   - Use external power for ESP32 during long scans
   - Monitor Flipper Zero battery

4. **Scan Duration**
   - 30-60 seconds typical for thorough scan
   - Longer scans find more networks

### For Handshake Capture

1. **Target Selection**
   - Ensure networks are detected first
   - First network is automatically targeted

2. **Be Patient**
   - Handshakes may take 10-30 seconds
   - Wait for clients to reconnect

3. **Check Results**
   - Verify packet count increased
   - Review capture file for EAPOL frames

4. **Legal Compliance**
   - Only target your own networks
   - Get written authorization for pen testing

### For Data Management

1. **Regular Backups**
   - Download captures frequently
   - SD card can fill up

2. **Organize Files**
   - Rename captures with dates/locations
   - Keep organized folders on computer

3. **Clean Up**
   - Delete old captures from SD card
   - Free up space for new data

### Safety Tips

1. **Battery**
   - Monitor Flipper Zero battery level
   - ESP32 can drain battery quickly

2. **Heat**
   - ESP32 may get warm during use
   - Normal, but ensure ventilation

3. **Connections**
   - Secure GPIO connections
   - Avoid shorts

## Troubleshooting Common Issues

### No Networks Found

- Check ESP32 connection
- Verify "ESP32" indicator
- Move to area with WiFi
- Restart app and try again

### Scan Doesn't Start

- Check if already scanning
- Press OK to stop, then start again
- Restart app if stuck

### Handshake Fails

- Ensure networks detected first
- Wait longer (some take time)
- Target may have no active clients
- Try different network

### Capture File Not Saved

- Check SD card is inserted
- Verify free space
- Check `/ext/apps_data/flipagotcha/` exists
- Try restarting app

For more help, see [Troubleshooting Guide](Troubleshooting.md).

## Advanced Usage

### Multiple Sessions

Run multiple scan sessions:
1. First session: general network scan
2. Second session: targeted handshake capture
3. Third session: specific channel monitoring

### Custom Analysis

Captured data can be:
- Imported into Wireshark
- Parsed with Python scripts
- Analyzed for security vulnerabilities
- Used for network auditing

### Integration with Other Tools

Captures can be used with:
- Hashcat (password cracking)
- Aircrack-ng (handshake analysis)
- Wireshark (packet analysis)
- Custom security tools

## Next Steps

- [Features](Features.md) - Learn about specific features
- [Troubleshooting](Troubleshooting.md) - Solve issues
- [FAQ](FAQ.md) - Common questions
- [Legal and Safety](Legal-and-Safety.md) - Important information

---

**Need Help?** Visit [Troubleshooting](Troubleshooting.md) or [GitHub Issues](https://github.com/1090mb/flipagotcha/issues)
