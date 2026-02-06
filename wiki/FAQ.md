# Frequently Asked Questions (FAQ)

Common questions and answers about Flipagotcha.

## Table of Contents

- [General Questions](#general-questions)
- [Hardware Questions](#hardware-questions)
- [Usage Questions](#usage-questions)
- [Technical Questions](#technical-questions)
- [Legal and Safety](#legal-and-safety)

## General Questions

### What is Flipagotcha?

Flipagotcha is a Pwnagotchi-style application for Flipper Zero that displays an animated face and integrates with ESP32 Marauder firmware for WiFi security research and packet capture.

### Do I need an ESP32 to use it?

**No!** The app works in two modes:
- **Demo Mode**: Works without ESP32, shows simulated data
- **ESP32 Mode**: Real WiFi scanning with ESP32 Marauder

Demo mode is perfect for testing the interface before getting hardware.

### Is this related to the original Pwnagotchi?

Inspired by, but not directly related. Pwnagotchi is a Raspberry Pi-based project. Flipagotcha brings similar concepts to Flipper Zero with its own implementation.

### Is this legal to use?

**It depends.** Using the app is legal. However:
- Scanning networks without permission may be illegal
- Deauthentication attacks are illegal in many jurisdictions
- Only use on networks you own or have authorization to test

See [Legal and Safety](Legal-and-Safety.md) for details.

### Is this safe for my Flipper Zero?

Yes! The app:
- Uses standard Flipper APIs
- Follows best practices
- Won't damage hardware
- Can be uninstalled cleanly

GPIO connections to ESP32 are safe when wired correctly.

### How much does it cost?

**Free!** Flipagotcha is open-source (MIT License).

Hardware costs:
- ESP32 board: $5-15
- Jumper wires: $2-5
- Optional: Custom board or enclosure

### Can I use this for penetration testing?

**Only with proper authorization!**
- Written permission from network owner
- Authorized penetration testing contracts
- Your own networks for security auditing

Unauthorized use is illegal and unethical.

---

## Hardware Questions

### What ESP32 boards are compatible?

Most ESP32 boards work:
- ESP32-WROOM-32 (most common)
- ESP32-S2
- ESP32-C3
- ESP32-WROVER
- Generic ESP32 dev boards

Key requirement: Must run Marauder firmware

### Where can I buy an ESP32?

Common sources:
- Amazon
- AliExpress
- Adafruit
- SparkFun
- Local electronics stores

Search for "ESP32 development board" or "ESP32-WROOM-32"

### What is Marauder firmware?

[ESP32 Marauder](https://github.com/justcallmekoko/ESP32Marauder) is open-source WiFi/Bluetooth penetration testing firmware for ESP32. It provides CLI commands for:
- WiFi scanning
- Packet capture
- Handshake capture
- Deauthentication
- Many other security tools

### How do I flash Marauder to ESP32?

See [Hardware Setup](Hardware-Setup.md) for detailed instructions.

Quick overview:
1. Download Marauder firmware
2. Install esptool: `pip install esptool`
3. Flash: `esptool.py --port COM3 write_flash 0x0 marauder.bin`

### Can I power ESP32 from Flipper Zero?

**Yes, but with caution:**
- Flipper's 3.3V pin provides limited current (~100mA)
- ESP32 can draw more, especially during WiFi transmission
- May cause brownouts or instability
- **Recommended**: Use external power for ESP32

### Do I need an antenna?

Depends on your ESP32 board:
- **Built-in antenna**: No external antenna needed
- **External antenna connector**: Yes, antenna required
- **Dual antenna**: Can use either

Check your board's specifications.

### Can I make a permanent connection?

Yes! Options:
- Solder wires directly
- Design custom PCB
- 3D print an enclosure
- Use a proto board

Many users create custom "WiFi Dev Boards" for Flipper.

### What if I wire it wrong?

**Swapping TX/RX**: Won't damage anything, just won't work. Swap them and try again.

**Wrong voltage**: Flipper and ESP32 both use 3.3V logic. Safe to connect directly.

**Short circuit**: Could damage devices. Double-check connections before powering on.

---

## Usage Questions

### How do I start a WiFi scan?

1. Launch Flipagotcha (Applications → GPIO)
2. Press **OK button** to start scanning
3. Watch network count increase
4. Press **OK** again to stop

### How long should I scan for?

**Typical**: 30-60 seconds
- Enough time to find most networks
- Captures sufficient packets
- Good balance of battery life

**Quick scan**: 10-20 seconds for nearby networks

**Thorough scan**: 2-3 minutes for distant networks

### What is handshake capture?

A **WPA handshake** is the authentication exchange between a WiFi router and client device. Capturing it allows:
- Network security testing
- Password strength verification
- Penetration testing (authorized only)

**To capture**:
1. Start scanning (OK button)
2. Wait for networks to be detected
3. Press **Left button** to initiate handshake capture
4. Wait 10-30 seconds
5. Stop scanning (OK button)

### How do I know if handshake capture succeeded?

Indicators:
- Packet count increases (20-50+ packets typical)
- No error messages
- Capture file contains EAPOL frames

**Note**: Not all handshakes succeed. Factors:
- Network must have active clients
- Client must reconnect during capture
- Router must support standard WPA

### Where are captured packets saved?

Location: `/ext/apps_data/flipagotcha/capture.txt`

Access via:
- qFlipper File Manager
- Remove SD card and read on computer
- Flipper File Browser app

### What format are captures saved in?

Custom text format with hex dumps:
```
Packet 0: len=64, ts=12345678, ch=6
00 01 02 03 04 05 06 07 ...
```

Not PCAP format (yet). Can be converted with custom scripts.

### Can I analyze captures with Wireshark?

Not directly. Captured packets are in text format, not PCAP.

**Workaround**:
1. Convert text to PCAP with custom script
2. Open PCAP in Wireshark

Future enhancement: Native PCAP export

### Why does it show "DEMO" mode?

Demo mode activates when:
- No ESP32 connected to GPIO
- ESP32 doesn't respond (not powered, wrong firmware)
- UART connection failed

Shows 3 simulated networks for testing interface.

### How do I switch from DEMO to ESP32 mode?

1. Exit app (Back button)
2. Connect ESP32 to GPIO pins correctly
3. Ensure ESP32 powered and running Marauder
4. Relaunch app
5. Should show "ESP32" in top-right

### Can I capture 5GHz networks?

**No.** ESP32 only supports 2.4GHz WiFi (802.11 b/g/n).

5GHz (802.11ac) requires different hardware not available in ESP32.

---

## Technical Questions

### What Flipper firmware version is required?

**Minimum**: 0.80.0 (approximately)

**Recommended**: Latest official firmware

Update via qFlipper for best compatibility.

### Does it work with custom firmware?

Should work with:
- Official firmware
- Unleashed firmware
- RogueMaster firmware
- Other forks

Built using standard Flipper SDK APIs.

### How much memory does it use?

Approximate:
- **App code**: ~50KB
- **RAM**: ~50KB for packet buffer
- **Stack**: 4KB
- **Total**: ~100KB RAM

Conservative for Flipper's ~128KB available RAM.

### How much battery does it use?

**Flipper Zero only**: Normal battery life

**With ESP32**:
- Powered from Flipper: 2-3 hours
- ESP32 externally powered: 4-6 hours (normal Flipper usage)

ESP32 WiFi radio is power-hungry.

### Can I run this with other apps?

**Simultaneously**: Not recommended
- Limited RAM
- GPIO conflicts

**Sequentially**: Yes
- Exit Flipagotcha
- Run other app
- No conflicts

### Does it work with qFlipper?

Yes! You can:
- Install FAP files via qFlipper
- Access File Manager
- Download capture files
- Update firmware

App runs on device, not through qFlipper.

### Can I modify the source code?

**Yes!** Flipagotcha is open-source (MIT License):
- View all source code
- Modify for your needs
- Fork and customize
- Contribute improvements

See [Contributing](Contributing.md) guide.

### How do I build from source?

See [Installation](Installation.md) for detailed instructions.

Quick version:
```bash
cd flipperzero-firmware
git clone <repo> applications_user/flipagotcha
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha
```

### Can I add custom face expressions?

**Yes!** Edit `src/ui/draw.c`:
- Face drawn with Canvas API
- Add new expressions
- Modify eye/mouth shapes
- Customize animations

Pure code, no image assets needed.

### What UART baud rate is used?

**115200 baud** (8N1)
- 8 data bits
- No parity
- 1 stop bit

Standard for Marauder firmware.

---

## Legal and Safety

### Is WiFi scanning legal?

**Generally yes**, but:
- Passive scanning (listening only) usually legal
- Active probing may have restrictions
- Laws vary by country
- Check local regulations

### Are deauthentication attacks legal?

**Usually NO!** Deauth attacks are:
- Illegal in most countries
- Violation of computer fraud laws
- Can result in prosecution
- Only legal with explicit authorization

**Never use without permission.**

### What is authorized use?

Legal authorized use:
- Your own networks and devices
- Written authorization from network owner
- Professional penetration testing contracts
- Security research in controlled environment
- Educational lab environments

### Can I get in trouble for having this app?

**Having the app**: Legal (it's just software)

**Using it improperly**: Can be illegal and result in:
- Criminal charges
- Fines
- Civil liability
- Loss of privileges

**Be responsible!** Only use with authorization.

### Is this a hacking tool?

It's a **security research tool**:
- Used by legitimate security professionals
- Educational purposes
- Testing your own network security
- Part of authorized penetration tests

Like any tool, can be misused. Use ethically and legally.

### What should I do before using it?

1. **Understand the law** in your jurisdiction
2. **Get authorization** for any network you test
3. **Document permission** in writing
4. **Use responsibly** and ethically
5. **Keep private** any data you capture

See [Legal and Safety](Legal-and-Safety.md) for complete information.

---

## Still Have Questions?

- Check other [wiki pages](Home.md)
- Read [Troubleshooting](Troubleshooting.md) guide
- Search [GitHub Issues](https://github.com/1090mb/flipagotcha/issues)
- Open a new issue if question not answered

---

**More Information**: [User Guide](User-Guide.md) | [Hardware Setup](Hardware-Setup.md) | [Troubleshooting](Troubleshooting.md)
