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

## 📂 Repository layout
(tree omitted for brevity – see the script source for the full list)

## 🛠️ Building the app
1. Clone this repo into your Flipper firmware tree under `applications_user/flipagotcha`.  
2. Run `./fbt COMPACT=1 APPSRC=applications_user/flipagotcha`.  
3. Flash the resulting `dist/flipagotcha.dfu` with qFlipper or `dfu-util`.

## 📡 Optional ESP‑32 Wi‑Fi bridge
(see design notes for the UART command set)

## ✨ WiFi Scanner Features (NEW)

The app now includes comprehensive WiFi scanning and packet capture capabilities:

### Features
- **Network Scanning**: Detect available WiFi networks in range
- **Handshake Capture**: Initiate WPA/WPA2 handshakes with detected networks  
- **Packet Capture**: Capture WiFi packets in real-time
- **Data Storage**: Save captured packets to `/ext/apps_data/flipagotcha/capture.txt`

### Updated Button Controls
- **OK** – Start/stop WiFi scanning and packet capture
- **← (Left)** – Initiate handshake with the first detected network
- **→ (Right)** – Open eyes manually  
- **↑ / ↓** – Switch between smile and frown

### On-Screen Display
- Network count and scanning status shown in top-left
- Packet count shown in bottom-left

For detailed technical documentation, see [WIFI_SCANNER.md](WIFI_SCANNER.md)

## 🎨 Adding more faces / expressions
Edit `src/ui/draw.c` – all drawing is done with Canvas primitives.

## 📜 License
MIT – see `LICENSE` file.
