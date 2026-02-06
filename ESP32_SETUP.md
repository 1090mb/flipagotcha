# ESP32 Marauder Setup Guide

This guide explains how to connect an ESP32 with Marauder firmware to your Flipper Zero via GPIO pins for WiFi scanning and packet capture.

## Hardware Requirements

- Flipper Zero
- ESP32 development board (ESP32-WROOM, ESP32-S2, or compatible)
- Jumper wires (female-to-female recommended)
- Optional: ESP32 WiFi Dev Board specifically designed for Flipper Zero

## GPIO Pin Connections

Connect the ESP32 to the Flipper Zero's GPIO pins as follows:

| Flipper Zero Pin | ESP32 Pin | Description |
|------------------|-----------|-------------|
| Pin 13 (TX)      | RX        | UART Transmit from Flipper |
| Pin 14 (RX)      | TX        | UART Receive to Flipper |
| Pin 8 (GND)      | GND       | Ground |
| Pin 1 (3.3V)     | 3.3V      | Power (if powering ESP32 from Flipper) |

**Important Notes:**
- The Flipper Zero uses 3.3V logic levels, which is compatible with ESP32
- If your ESP32 draws too much power, use an external power source for the ESP32 and only connect TX, RX, and GND
- Ensure proper ground connection between devices

## ESP32 Marauder Firmware

### What is Marauder?

ESP32 Marauder is a suite of WiFi/Bluetooth offensive and defensive tools for the ESP32. It provides a CLI interface over UART for:
- WiFi network scanning
- Packet sniffing and capture
- WPA/WPA2 handshake capture
- Deauthentication attacks
- Beacon spam detection
- And more...

### Flashing Marauder Firmware

#### Option 1: Using ESP Flash Download Tool (Windows)

1. Download ESP32 Marauder firmware from: https://github.com/justcallmekoko/ESP32Marauder/releases
2. Download ESP Flash Download Tool: https://www.espressif.com/en/support/download/other-tools
3. Connect ESP32 to your computer via USB
4. Run the Flash Download Tool
5. Select the appropriate firmware files and flash addresses
6. Click "START" to flash the firmware

#### Option 2: Using esptool.py (Cross-platform)

```bash
# Install esptool
pip install esptool

# Download the firmware
wget https://github.com/justcallmekoko/ESP32Marauder/releases/latest/download/esp32_marauder.bin

# Erase the flash (optional but recommended)
esptool.py --port /dev/ttyUSB0 erase_flash

# Flash the firmware
esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash 0x0 esp32_marauder.bin
```

#### Option 3: Web Flasher

Visit the official ESP32 Marauder web flasher (if available) for the easiest installation method.

### Verifying Marauder Installation

1. Connect ESP32 to your computer via USB
2. Open a serial terminal (115200 baud, 8N1)
   - Windows: PuTTY, TeraTerm
   - Linux/Mac: `screen /dev/ttyUSB0 115200`
3. Type `help` and press Enter
4. You should see a list of available Marauder commands

## Using Flipagotcha with ESP32 Marauder

### Connection Status

When you launch the Flipagotcha app:
- If ESP32 is connected and responding: "ESP32" indicator in top-right
- If ESP32 is not connected: "DEMO" mode with mock data

### Button Controls

| Button | Function |
|--------|----------|
| OK     | Start/Stop WiFi scanning and packet capture |
| Left   | Initiate WPA handshake capture with first detected network |
| Right  | Open eyes manually |
| Up     | Change to smile |
| Down   | Change to frown |
| Back   | Exit app |

### WiFi Scanning Workflow

1. **Start Scanning**
   - Press OK button to start WiFi network scan
   - The app sends "scanap" command to ESP32 Marauder
   - Networks are displayed as they're discovered
   - Packet capture starts automatically

2. **Capture Handshake**
   - Press Left button to capture WPA/WPA2 handshake for the first network
   - The app sends "sniffpmkid -c <channel> -d" command
   - Deauthentication frames force clients to reconnect
   - EAPOL handshake frames are captured

3. **Stop and Save**
   - Press OK button again to stop scanning
   - Captured packets are automatically saved to:
     `/ext/apps_data/flipagotcha/capture.txt`

### Marauder Commands Used

The app automatically sends these commands to Marauder:

- `scanap` - Scan for WiFi access points
- `sniffpmkid -c <channel> -d` - Capture PMKID/handshake with deauth
- `sniffraw` - Capture all raw 802.11 frames
- `stopscan` - Stop current operation

## Troubleshooting

### ESP32 Not Detected

1. **Check UART Connection**
   - Verify TX/RX pins are correctly connected
   - Ensure ground is connected
   - Try swapping TX/RX if not working

2. **Check Power**
   - ESP32 should be powered (LED indicator on most boards)
   - If powered externally, ensure common ground

3. **Verify Marauder Firmware**
   - Connect ESP32 to computer
   - Open serial terminal at 115200 baud
   - Type `help` - should see command list

4. **Check UART Settings**
   - Flipagotcha uses 115200 baud, 8N1
   - Marauder default is also 115200 baud

### No Networks Detected

1. **ESP32 Antenna**
   - Ensure ESP32 has antenna attached (if external antenna model)
   - Built-in antenna models should work out of the box

2. **WiFi Environment**
   - Move to area with known WiFi networks
   - 2.4GHz networks only (ESP32 doesn't support 5GHz)

3. **Marauder Scanning**
   - Test directly via serial terminal: `scanap`
   - Should see networks listed

### Captured Data

Captured packets are saved in text format at:
```
/ext/apps_data/flipagotcha/capture.txt
```

Format:
```
# Flipagotcha Packet Capture
# Packets: <count>

Packet 0: len=<length>, ts=<timestamp>, ch=<channel>
<hex dump of packet data>
...
```

## Advanced Usage

### Custom Marauder Commands

While Flipagotcha automates common operations, you can also:
1. Use a serial terminal app on Flipper Zero
2. Connect to ESP32 UART manually
3. Send any Marauder command directly

### Channel Hopping

Marauder automatically hops between WiFi channels during scanning to discover all networks.

### PCAP Export

For advanced analysis, captured data can be:
1. Retrieved from SD card
2. Converted to PCAP format
3. Analyzed with Wireshark

## Safety and Legal Considerations

⚠️ **WARNING**: This tool is for educational and authorized security testing only.

- Only scan networks you own or have explicit permission to test
- Deauthentication attacks may be illegal in your jurisdiction
- Packet capture of networks you don't own may violate privacy laws
- Always ensure you have proper authorization

## Resources

- [ESP32 Marauder GitHub](https://github.com/justcallmekoko/ESP32Marauder)
- [Marauder Wiki](https://github.com/justcallmekoko/ESP32Marauder/wiki)
- [Marauder CLI Commands](https://github.com/justcallmekoko/ESP32Marauder/wiki/cli)
- [Flipper Zero GPIO Documentation](https://docs.flipperzero.one/gpio-and-modules)

## Support

For issues specific to:
- **Flipagotcha app**: Open an issue on this repository
- **ESP32 Marauder**: Visit the Marauder GitHub repository
- **Flipper Zero hardware**: Visit Flipper Zero documentation

## Demo Mode

If you don't have an ESP32 with Marauder firmware, the app works in DEMO mode with mock data:
- 3 simulated WiFi networks
- Simulated packet captures
- All UI features functional

This allows you to test the app interface before setting up the hardware.
