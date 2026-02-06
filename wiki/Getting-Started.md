# Getting Started with Flipagotcha

This quick start guide will help you get Flipagotcha up and running on your Flipper Zero in just a few minutes.

## Prerequisites

Before you begin, make sure you have:

- ✅ **Flipper Zero** device with updated firmware
- ✅ **Computer** for building the application
- ✅ **USB cable** to connect Flipper Zero
- ✅ **SD card** installed in Flipper Zero (for saving captures)
- ⚪ **ESP32 board** with Marauder firmware (optional - app works in demo mode without it)

## Quick Installation

### Method 1: Pre-built Binary (Recommended)

1. **Download the latest release**
   - Visit the [Releases page](https://github.com/1090mb/flipagotcha/releases)
   - Download `flipagotcha.fap` file

2. **Install to Flipper Zero**
   - Connect Flipper Zero to your computer
   - Copy `flipagotcha.fap` to `/ext/apps/GPIO/` on your Flipper Zero SD card
   - Alternatively, use qFlipper to install the FAP file

3. **Launch the app**
   - On your Flipper Zero: Go to **Applications** → **GPIO** → **Flipagotcha**

### Method 2: Build from Source

1. **Clone the repository**
   ```bash
   cd /path/to/flipper-firmware/applications_user/
   git clone https://github.com/1090mb/flipagotcha.git
   ```

2. **Build the application**
   ```bash
   cd /path/to/flipper-firmware/
   ./fbt COMPACT=1 APPSRC=applications_user/flipagotcha
   ```

3. **Install the built app**
   - The compiled `.fap` file will be in the `dist/` directory
   - Use qFlipper or copy manually to Flipper Zero SD card

For detailed installation instructions, see the [Installation Guide](Installation.md).

## First Launch

### Without ESP32 (Demo Mode)

1. Launch the Flipagotcha app
2. You'll see an animated face on the screen
3. The top-right corner will show **"DEMO"** indicator
4. Try the controls:
   - **OK button**: Start/stop demo WiFi scanning
   - **Left/Right**: Close/open eyes
   - **Up/Down**: Smile/frown
5. Demo mode shows 3 simulated networks

### With ESP32 Marauder

1. Connect ESP32 to Flipper Zero GPIO pins (see below)
2. Launch the Flipagotcha app
3. The top-right corner will show **"ESP32"** indicator
4. You're ready for real WiFi scanning!

## Quick ESP32 Setup

If you have an ESP32 with Marauder firmware:

### Connection
Connect these GPIO pins on Flipper Zero to ESP32:

| Flipper Pin | ESP32 Pin | Purpose |
|-------------|-----------|---------|
| 13 (TX)     | RX        | Data from Flipper to ESP32 |
| 14 (RX)     | TX        | Data from ESP32 to Flipper |
| 8 (GND)     | GND       | Common ground |

For complete ESP32 setup instructions, see [Hardware Setup Guide](Hardware-Setup.md).

## Basic Usage

### Button Controls

- **OK Button** - Start/stop WiFi scanning
- **Left Button** - Capture handshake from first detected network
- **Right Button** - Open eyes manually
- **Up Button** - Change to smile
- **Down Button** - Change to frown
- **Back Button** - Exit application

### What You'll See

- **Top-left**: Network count and scanning status
- **Top-right**: Connection status (ESP32 or DEMO)
- **Center**: Animated face with expressions
- **Bottom-left**: Captured packet count

### Your First WiFi Scan

1. **Press OK** to start scanning
   - Watch the network count increase
   - Packet capture starts automatically

2. **Press Left** (optional) to capture a handshake
   - Targets the first detected network
   - Sends deauthentication frames to trigger handshake

3. **Press OK** again to stop
   - Packets are automatically saved to:
     `/ext/apps_data/flipagotcha/capture.txt`

## What's Next?

Now that you have Flipagotcha running:

- 📖 Read the [User Guide](User-Guide.md) for detailed usage instructions
- 🔧 Set up [ESP32 Hardware](Hardware-Setup.md) for real WiFi scanning
- 🎯 Learn about all [Features](Features.md)
- ❓ Check the [FAQ](FAQ.md) for common questions
- 🐛 Visit [Troubleshooting](Troubleshooting.md) if you encounter issues

## Important Notes

### Demo Mode
- Without ESP32, the app runs in **demo mode**
- Shows 3 simulated WiFi networks
- Useful for testing the interface
- All buttons and features work normally

### Legal Notice
⚠️ **Always get permission before scanning WiFi networks!**

This tool is for:
- Educational purposes
- Your own networks
- Authorized security testing only

Unauthorized use may be illegal in your jurisdiction. See [Legal and Safety](Legal-and-Safety.md) for details.

## Need Help?

- 🐛 [Troubleshooting Guide](Troubleshooting.md)
- ❓ [FAQ](FAQ.md)
- 💬 [GitHub Issues](https://github.com/1090mb/flipagotcha/issues)
- 📧 Contact the maintainer

## Additional Resources

- [Full User Guide](User-Guide.md)
- [Features Documentation](Features.md)
- [Architecture Details](Architecture.md)
- [ESP32 Marauder Documentation](https://github.com/justcallmekoko/ESP32Marauder/wiki)

---

**Next Steps**: [Installation Guide](Installation.md) | [Hardware Setup](Hardware-Setup.md) | [User Guide](User-Guide.md)
