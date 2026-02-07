# flipagotcha – a tiny “Pwnagotchi‑style” Flipper Zero app

**What it does**

* Shows a minimalist animated face (blinking eyes, smile/frown) on the Flipper’s 128 × 64 monochrome screen.  
* Uses UART to talk to an external ESP‑32 (optional) for Wi‑Fi sniffing.  
* Buttons:  
  * **OK** – toggles the Wi‑Fi scanner (`CMD_TOGGLE_SCAN`).  
  * **← / →** – close/open eyes manually.  
  * **↑ / ↓** – switch between smile and frown.  

**Why no PNG assets?**  
The face is drawn directly with the Flipper Canvas API, so the binary stays tiny and you never need to generate or ship image files.

## 📚 Complete Documentation

**Quick Links:**
- 🎯 **[Quick Reference Card](QUICK_REFERENCE.md)** - Essential controls and workflows
- 🎮 **[UI Guide](UI_GUIDE.md)** - Visual layout and navigation
- ⚔️ **[Bettercap Features](BETTERCAP_FEATURES.md)** - Attack modes and capabilities
- 📡 **[ESP32 Setup](ESP32_SETUP.md)** - Hardware configuration
- 🔧 **[WiFi Scanner Technical](WIFI_SCANNER.md)** - Implementation details

**[Visit the Wiki](wiki/Home.md)** for comprehensive documentation including:
- 🚀 [Getting Started Guide](wiki/Getting-Started.md)
- 📦 [Installation Instructions](wiki/Installation.md)
- 🔧 [Hardware Setup](wiki/Hardware-Setup.md)
- 📖 [User Guide](wiki/User-Guide.md)
- ⚡ [Features](wiki/Features.md)
- 🏗️ [Architecture](wiki/Architecture.md)
- 📝 [API Reference](wiki/API-Reference.md)
- 🐛 [Troubleshooting](wiki/Troubleshooting.md)
- 🤝 [Contributing Guide](wiki/Contributing.md)
- ❓ [FAQ](wiki/FAQ.md)
- ⚖️ [Legal and Safety](wiki/Legal-and-Safety.md)

## 📂 Repository layout
(tree omitted for brevity – see the script source for the full list)

## 🛠️ Building the app
1. Clone this repo into your Flipper firmware tree under `applications_user/flipagotcha`.  
2. Run `./fbt COMPACT=1 APPSRC=applications_user/flipagotcha`.  
3. Flash the resulting `dist/flipagotcha.dfu` with qFlipper or `dfu-util`.

## 📡 ESP32 Marauder WiFi Integration

The Flipagotcha app connects to an ESP32 with **Marauder firmware** via GPIO UART pins to perform real WiFi scanning and packet capture operations.

### Quick Start
1. Flash ESP32 with [Marauder firmware](https://github.com/justcallmekoko/ESP32Marauder)
2. Connect ESP32 to Flipper Zero GPIO pins:
   - Flipper TX (Pin 13) → ESP32 RX
   - Flipper RX (Pin 14) → ESP32 TX
   - GND (Pin 8) → ESP32 GND
3. Launch Flipagotcha app
4. Look for "ESP32" indicator in top-right corner

For detailed setup instructions, see **[ESP32_SETUP.md](ESP32_SETUP.md)**

### Demo Mode
No ESP32? The app runs in DEMO mode with simulated data, so you can test the interface without hardware.

## ✨ Advanced Features - Bettercap-Inspired Attacks (NEW!)

The app now includes powerful bettercap-inspired attack capabilities and an enhanced menu system:

### 🎯 Attack Capabilities
- **Deauth Attacks**: Send deauthentication frames to disconnect clients from APs
  - Target specific networks or broadcast to all
  - Real-time attack status display
- **Beacon Spam**: Inject fake beacon frames to create spoofed networks
- **PMKID Capture**: Specialized mode for capturing PMKID hashes from WPA/WPA2 networks
- **Channel Hopping**: Automatically cycle through WiFi channels for better coverage

### 📊 Session Statistics
Track your WiFi security testing in real-time:
- Handshakes captured
- Deauth frames sent
- Beacon frames injected
- PMKIDs captured
- Total packets captured

### 🎨 Dynamic Face Expressions
The face changes based on the app's state:
- **Happy** (^_^) - Successfully captured packets
- **Attacking** (>_<) - Active attack in progress
- **Scanning** (•_•) - Scanning for networks
- **Confused** (x_x) - ESP32 not connected
- **Normal** (•‿•) - Ready and waiting

### 🎛️ Enhanced Menu System
Navigate with the directional buttons from the main screen:

- **Left Arrow** → **Attack Menu**
  - Launch deauth attacks
  - Start beacon spam
  - Enable PMKID capture
  - Toggle channel hopping
  - Stop active attacks

- **Right Arrow** → **Statistics View**
  - Real-time capture statistics
  - Session performance metrics
  - Attack success tracking

- **Up Arrow** → **Packet Filter Menu**
  - Filter by packet type (Beacon, Probe, Data, Deauth, EAPOL)
  - Customize what packets to capture
  - Optimize storage and processing

- **Down Arrow** → **Settings Menu**
  - Configure auto-save intervals
  - Audio feedback options
  - Channel preferences
  - (More options coming soon)

### 📱 Updated Button Controls
- **OK** – Start/stop WiFi scanning and packet capture
- **← (Left)** – Open Attack Menu
- **→ (Right)** – View Statistics
- **↑ (Up)** – Open Packet Filter Menu
- **↓ (Down)** – Open Settings Menu
- **Back** – Exit app or return to main screen

### 📈 On-Screen Display
- Network count and scanning status (top-left)
- Active attack indicator (if attacking)
- Packet count (bottom-left)
- ESP32 connection status (top-right)

### 💾 Data Storage
- Captured packets saved to `/ext/apps_data/flipagotcha/capture.txt`
- Session statistics tracked automatically
- Support for multiple capture sessions

For detailed technical documentation, see [WIFI_SCANNER.md](WIFI_SCANNER.md)

## 🎨 Adding more faces / expressions
Edit `src/ui/draw.c` – all drawing is done with Canvas primitives.

## ⚠️ Legal Notice

This tool is for **educational purposes and authorized security testing only**. 

**Always ensure you have proper authorization before scanning or capturing packets from any network.** Unauthorized use may be illegal in your jurisdiction.

See **[Legal and Safety](wiki/Legal-and-Safety.md)** for complete information.

## 📜 License
MIT – see `LICENSE` file.
