# Hardware Setup Guide

This guide explains how to connect an ESP32 board with Marauder firmware to your Flipper Zero for real WiFi scanning capabilities.

## Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [ESP32 Marauder Firmware](#esp32-marauder-firmware)
- [GPIO Pin Connections](#gpio-pin-connections)
- [Flashing Marauder Firmware](#flashing-marauder-firmware)
- [Testing the Connection](#testing-the-connection)
- [Power Considerations](#power-considerations)
- [Troubleshooting](#troubleshooting)

## Overview

Flipagotcha communicates with an ESP32 running Marauder firmware via UART to perform WiFi operations. The ESP32 handles:

- WiFi network scanning
- Packet capture
- WPA/WPA2 handshake capture
- Deauthentication attacks
- Channel hopping

**Note**: The app works in demo mode without ESP32 hardware, using simulated data for testing.

## Hardware Requirements

### Required Components

1. **ESP32 Development Board**
   - ESP32-WROOM-32 (most common)
   - ESP32-S2
   - ESP32-C3
   - Or compatible ESP32 variant

2. **Flipper Zero** with SD card

3. **Jumper Wires**
   - Female-to-female jumper wires (3-4 wires needed)
   - Or a custom connector if using a specialized WiFi board

### Recommended ESP32 Boards

- **Generic ESP32 DevKit** - Budget-friendly, widely available
- **ESP32-WROOM-32** - Standard development board
- **Flipper Zero WiFi DevBoard** - Pre-configured for Flipper Zero (if available)
- **ESP32-S2-WROVER** - Enhanced WiFi capabilities

### Optional Components

- **External Power Source** - If ESP32 draws too much power
- **Antenna** - Some ESP32 boards need external antenna
- **Enclosure** - To protect and mount the ESP32

## ESP32 Marauder Firmware

### What is Marauder?

[ESP32 Marauder](https://github.com/justcallmekoko/ESP32Marauder) is an open-source WiFi/Bluetooth penetration testing tool for ESP32. It provides:

- Text-based CLI over UART
- WiFi scanning and monitoring
- Packet capture capabilities
- WPA handshake capture
- Deauthentication attacks
- And many more features

### Supported Commands

Flipagotcha uses these Marauder commands:

- `scanap` - Scan for WiFi access points
- `sniffpmkid -c <channel> -d` - Capture handshakes with deauth
- `sniffraw` - Capture raw 802.11 packets
- `stopscan` - Stop current operation
- `help` - List available commands (used for connection detection)

## GPIO Pin Connections

### Flipper Zero GPIO Pinout

Connect these pins on Flipper Zero's GPIO header to your ESP32:

| Pin # | Function | Color (typical) | Description |
|-------|----------|-----------------|-------------|
| 1     | 3.3V     | Red             | Power output (use with caution) |
| 8     | GND      | Black           | Ground |
| 13    | TX       | White/Yellow    | UART Transmit (Flipper → ESP32) |
| 14    | RX       | Blue/Green      | UART Receive (ESP32 → Flipper) |

### Connection Table

| Flipper Zero Pin | ESP32 Pin | Wire Purpose |
|------------------|-----------|--------------|
| Pin 13 (TX)      | RX (GPIO3 typically) | Data from Flipper to ESP32 |
| Pin 14 (RX)      | TX (GPIO1 typically) | Data from ESP32 to Flipper |
| Pin 8 (GND)      | GND | Common ground reference |
| Pin 1 (3.3V)     | 3.3V or VIN | Power (optional, see notes) |

### Visual Connection Diagram

```
Flipper Zero GPIO               ESP32 Board
┌──────────────┐               ┌──────────────┐
│ Pin 1 (3.3V) ├──────────────→│ 3.3V / VIN   │ (Optional)
│              │               │              │
│ Pin 8 (GND)  ├──────────────→│ GND          │ (Required)
│              │               │              │
│ Pin 13 (TX)  ├──────────────→│ RX (GPIO3)   │ (Required)
│              │               │              │
│ Pin 14 (RX)  ←──────────────┤ TX (GPIO1)   │ (Required)
└──────────────┘               └──────────────┘
```

### Important Notes

⚠️ **UART Connection**
- TX on Flipper connects to RX on ESP32
- RX on Flipper connects to TX on ESP32
- Always connect GND between devices

⚠️ **Power Considerations**
- Flipper Zero's 3.3V pin has limited current capacity (~100mA)
- Some ESP32 boards draw more power, especially during WiFi operations
- If connection is unstable, use external power for ESP32
- When using external power, still connect GND between devices

⚠️ **Voltage Levels**
- Both devices use 3.3V logic levels - no level shifter needed
- Never connect 5V signals directly to Flipper Zero

## Flashing Marauder Firmware

### Method 1: Using esptool.py (Cross-platform)

#### Step 1: Install esptool

```bash
# Using pip
pip install esptool

# Or using pip3
pip3 install esptool
```

#### Step 2: Download Marauder Firmware

1. Visit [ESP32 Marauder Releases](https://github.com/justcallmekoko/ESP32Marauder/releases)
2. Download the appropriate firmware for your ESP32 model:
   - `esp32_marauder_v[version]_[board].bin`

#### Step 3: Erase Flash (Recommended)

```bash
# Replace /dev/ttyUSB0 with your ESP32's serial port
# Windows: COM3, COM4, etc.
# macOS: /dev/cu.usbserial-*
# Linux: /dev/ttyUSB0, /dev/ttyACM0, etc.

esptool.py --port /dev/ttyUSB0 erase_flash
```

#### Step 4: Flash Firmware

```bash
# Flash the firmware
esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash 0x0 esp32_marauder_v[version].bin

# Wait for completion
# Should see: "Hash of data verified."
```

### Method 2: Using ESP Flash Download Tool (Windows)

1. **Download ESP Flash Download Tool**
   - Visit [Espressif Tools](https://www.espressif.com/en/support/download/other-tools)
   - Download "Flash Download Tools"

2. **Connect ESP32 to Computer**
   - Connect via USB
   - Note the COM port (Device Manager → Ports)

3. **Configure Flash Tool**
   - Select ESP32 chip type
   - Load the Marauder `.bin` file
   - Set flash address: `0x0`
   - Select your COM port
   - Set baud rate: 460800

4. **Flash**
   - Click "START"
   - Wait for "FINISH" message

### Method 3: Web Flasher (Easiest)

Some versions of Marauder offer a web-based flasher:

1. Visit the Marauder web flasher (check project documentation)
2. Connect ESP32 via USB
3. Follow on-screen instructions
4. Browser-based, no installation needed

### Verifying Marauder Installation

After flashing:

1. **Connect ESP32 to computer via USB**

2. **Open serial terminal**
   ```bash
   # Linux/macOS
   screen /dev/ttyUSB0 115200
   
   # Or use minicom
   minicom -D /dev/ttyUSB0 -b 115200
   
   # Windows: Use PuTTY or TeraTerm
   ```

3. **Test Marauder**
   - Type: `help`
   - Press Enter
   - You should see a list of Marauder commands

4. **Test WiFi scanning**
   - Type: `scanap`
   - Should see WiFi networks being discovered

## Testing the Connection

### With Flipagotcha App

1. **Connect ESP32 to Flipper Zero GPIO**
   - Follow the pinout diagram above
   - Double-check TX/RX are correctly crossed
   - Ensure GND is connected

2. **Power on both devices**

3. **Launch Flipagotcha**
   - Navigate to: Applications → GPIO → Flipagotcha
   - Press OK to launch

4. **Check connection indicator**
   - Look at top-right corner of screen
   - **"ESP32"** = Successfully connected
   - **"DEMO"** = Not connected (demo mode)

5. **Test WiFi scanning**
   - Press OK to start scan
   - Should see real networks if ESP32 connected
   - Network count should increase

### Manual UART Testing

If connection doesn't work, test UART directly:

1. **Use a serial terminal on Flipper Zero**
   - Install UART Terminal app
   - Configure: 115200 baud, 8N1
   - TX: Pin 13, RX: Pin 14

2. **Send test command**
   - Type: `help`
   - Should see Marauder command list

## Power Considerations

### Option 1: Power from Flipper Zero

**Pros:**
- Simple, single power source
- No extra batteries needed

**Cons:**
- Limited current (~100mA from 3.3V pin)
- May cause brownouts during WiFi transmission
- Can drain Flipper Zero battery faster

**When to use:**
- Low-power ESP32 boards
- Short scanning sessions
- Testing/development

### Option 2: External Power for ESP32

**Pros:**
- Stable power during WiFi operations
- Doesn't drain Flipper Zero battery
- Better for continuous operation

**Cons:**
- Need separate power source
- More complex wiring

**Connection:**
```
Flipper Zero              ESP32
Pin 13 (TX) ──────────→ RX
Pin 14 (RX) ←────────── TX
Pin 8 (GND) ──────────→ GND
                          ↓
                      3.3V from USB or battery
```

**When to use:**
- Extended scanning sessions
- High-power ESP32 boards
- Production use

### Option 3: Battery Pack for ESP32

- Use a small 3.7V LiPo battery
- Add voltage regulator for 3.3V
- Portable solution for field use

## Troubleshooting

### ESP32 Not Detected

**Check these:**

1. **Wiring**
   - TX/RX correctly crossed
   - GND connected
   - No loose connections

2. **Power**
   - ESP32 LED indicator on
   - Try external power source
   - Check voltage with multimeter

3. **Marauder Firmware**
   - Re-flash firmware
   - Try different baud rate (115200 is standard)
   - Verify with serial terminal on computer

4. **UART Configuration**
   - Some ESP32 boards use different GPIO pins for UART
   - Check your board's pinout diagram
   - May need to use different GPIO pins

### Connection Unstable

**Solutions:**

1. **Use external power** for ESP32
2. **Shorter wires** - reduce interference
3. **Proper ground** - ensure good GND connection
4. **Add capacitor** - 10µF across ESP32 power pins

### No Networks Detected

1. **Check antenna** - External antenna boards need antenna connected
2. **Test with computer** - Connect ESP32 via USB, run `scanap`
3. **WiFi environment** - Move to area with known 2.4GHz networks
4. **Firmware version** - Update to latest Marauder version

### Commands Not Working

1. **Verify Marauder** - Test `help` command
2. **Check baud rate** - Should be 115200
3. **UART buffer** - Power cycle both devices
4. **Cable quality** - Try different jumper wires

## Advanced Configuration

### Custom UART Pins on ESP32

If you need to use different GPIO pins:

1. Modify Marauder firmware source
2. Recompile with desired GPIO assignments
3. Reflash to ESP32

### Permanent Installation

For a cleaner setup:

1. **Solder connections** instead of jumper wires
2. **Design custom PCB** for Flipper GPIO
3. **3D print enclosure** for both devices
4. **Add power switch** for ESP32

### Multiple ESP32 Configurations

Some users keep multiple ESP32 boards for different purposes:
- One for packet capture
- One for deauth attacks
- One for beacon spam

Swap boards as needed for different tasks.

## Safety and Legal

⚠️ **Important Warnings:**

- WiFi scanning and deauth attacks may be illegal without authorization
- Only use on networks you own or have permission to test
- Follow local laws and regulations
- Educational and authorized testing purposes only

See [Legal and Safety](Legal-and-Safety.md) for complete information.

## Resources

- [ESP32 Marauder GitHub](https://github.com/justcallmekoko/ESP32Marauder)
- [Marauder Wiki](https://github.com/justcallmekoko/ESP32Marauder/wiki)
- [Marauder CLI Reference](https://github.com/justcallmekoko/ESP32Marauder/wiki/cli)
- [Flipper Zero GPIO Docs](https://docs.flipperzero.one/gpio-and-modules)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)

## Next Steps

After successful hardware setup:

1. [User Guide](User-Guide.md) - Learn how to use all features
2. [Features](Features.md) - Explore WiFi scanning capabilities
3. [Troubleshooting](Troubleshooting.md) - Solve common issues

---

**Need Help?** Visit [Troubleshooting](Troubleshooting.md) or [GitHub Issues](https://github.com/1090mb/flipagotcha/issues)
