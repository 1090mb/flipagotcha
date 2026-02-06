# Troubleshooting

Solutions to common issues and problems with Flipagotcha.

## Table of Contents

- [Quick Diagnostics](#quick-diagnostics)
- [Installation Issues](#installation-issues)
- [Connection Problems](#connection-problems)
- [Scanning Issues](#scanning-issues)
- [Capture Problems](#capture-problems)
- [File and Storage Issues](#file-and-storage-issues)
- [Performance Issues](#performance-issues)
- [ESP32 Marauder Issues](#esp32-marauder-issues)
- [Getting More Help](#getting-more-help)

## Quick Diagnostics

### Check List

Before troubleshooting, verify:

- [ ] Flipper Zero firmware is up to date
- [ ] SD card is properly inserted
- [ ] SD card has free space
- [ ] `flipagotcha.fap` is in `/ext/apps/GPIO/`
- [ ] ESP32 connections are secure (if using hardware)
- [ ] ESP32 has Marauder firmware (if using hardware)

### Status Indicators

**Top-right corner of screen:**
- **"ESP32"** = Hardware connected and working
- **"DEMO"** = Running in demo mode (no hardware or connection failed)

**Bottom-left corner:**
- **"Packets: N"** = Number of captured packets

---

## Installation Issues

### Issue: App doesn't appear in menu

**Symptoms:**
- Flipagotcha not in Applications → GPIO menu
- Can't find the app

**Solutions:**

1. **Check file location**
   ```
   File must be: /ext/apps/GPIO/flipagotcha.fap
   NOT: /ext/apps/flipagotcha.fap
   ```

2. **Verify SD card**
   - Reinsert SD card
   - Check for errors: Settings → Storage → SD Info

3. **Restart Flipper Zero**
   - Long press Back button
   - Select "Reboot"

4. **Rebuild/redownload**
   - Get fresh `.fap` file
   - Check file size is reasonable (~100KB+)

---

### Issue: "Failed to load application"

**Symptoms:**
- App appears in menu but won't launch
- Error message on screen

**Solutions:**

1. **Update firmware**
   - Use qFlipper to update to latest firmware
   - Flipagotcha may require newer firmware version

2. **Check FAP compatibility**
   - Ensure `.fap` file matches your firmware version
   - Rebuild from source if needed

3. **SD card corruption**
   - Backup data
   - Format SD card (FAT32)
   - Reinstall app

---

### Issue: App crashes immediately

**Symptoms:**
- App launches then returns to menu
- Screen flickers
- Flipper restarts

**Solutions:**

1. **Check SD card**
   - Ensure card is not full
   - Test with different SD card

2. **Free up memory**
   - Close other apps
   - Restart Flipper Zero

3. **Rebuild with debug**
   ```bash
   ./fbt DEBUG=1 APPSRC=applications_user/flipagotcha
   ```
   - Check logs for error messages

---

## Connection Problems

### Issue: ESP32 not detected (shows "DEMO")

**Symptoms:**
- Top-right shows "DEMO" instead of "ESP32"
- Demo mode with 3 fake networks
- Real scanning doesn't work

**Solutions:**

1. **Check wiring**
   ```
   Flipper Pin 13 (TX) → ESP32 RX
   Flipper Pin 14 (RX) → ESP32 TX
   Flipper Pin 8 (GND) → ESP32 GND
   ```
   - Verify TX and RX are crossed
   - Check for loose connections

2. **Check ESP32 power**
   - ESP32 LED should be on
   - Try external power source
   - Measure voltage (3.3V expected)

3. **Test with serial terminal**
   - Connect ESP32 to computer
   - Open serial terminal (115200 baud)
   - Type `help` - should see commands
   - If no response, reflash Marauder

4. **Verify Marauder firmware**
   - Latest version from [ESP32 Marauder releases](https://github.com/justcallmekoko/ESP32Marauder/releases)
   - Correct board variant

5. **Try different jumper wires**
   - Poor quality wires can cause issues
   - Short wires work better than long

---

### Issue: Connection intermittent

**Symptoms:**
- Sometimes shows "ESP32", sometimes "DEMO"
- Connection drops during use
- Unreliable scanning

**Solutions:**

1. **Power stability**
   - Use external power for ESP32
   - Add 10µF capacitor across ESP32 power pins
   - Check battery level on both devices

2. **Reduce wire length**
   - Shorter wires = more reliable
   - Keep wires away from interference sources

3. **Secure connections**
   - Ensure pins fully inserted
   - Consider soldering for permanent setup

4. **Check ground**
   - GND connection is critical
   - Verify continuity with multimeter

---

## Scanning Issues

### Issue: No networks detected

**Symptoms:**
- Network count stays at 0
- Scanning seems to run but finds nothing
- "ESP32" mode active

**Solutions:**

1. **Check environment**
   - Move to area with known WiFi networks
   - 2.4GHz networks only (ESP32 doesn't support 5GHz)
   - Try near a known router

2. **ESP32 antenna**
   - Some boards need external antenna attached
   - Check antenna connection
   - Verify antenna is designed for 2.4GHz

3. **Test Marauder directly**
   - Connect ESP32 to computer
   - Serial terminal: `scanap`
   - Should list networks
   - If not, issue is with Marauder/ESP32

4. **Channel issues**
   - Some regions use different channels
   - Marauder scans channels 1-14 by default

---

### Issue: Scan doesn't start

**Symptoms:**
- Press OK but nothing happens
- Network count doesn't appear
- No activity

**Solutions:**

1. **Check if already scanning**
   - Press OK to stop current scan
   - Wait a moment
   - Press OK again to start fresh

2. **Restart app**
   - Press Back to exit
   - Relaunch from menu
   - Try again

3. **ESP32 timeout**
   - ESP32 may be unresponsive
   - Power cycle ESP32
   - Relaunch app

---

### Issue: Scan count stuck

**Symptoms:**
- Network count frozen
- Doesn't increase
- Appears to be stuck

**Solutions:**

1. **Stop and restart**
   - Press OK to stop
   - Wait 5 seconds
   - Press OK to start new scan

2. **ESP32 buffer overflow**
   - Stop scanning
   - Power cycle ESP32
   - Restart app

---

## Capture Problems

### Issue: Handshake capture fails

**Symptoms:**
- Press Left button
- Packet count doesn't increase
- No handshake captured

**Solutions:**

1. **Ensure networks detected first**
   - Must scan and find networks before handshake
   - Network count should be > 0

2. **Target may have no clients**
   - Handshake requires active client connection
   - Try different network (multiple scans)
   - Wait longer (30+ seconds)

3. **Wrong channel**
   - Network may have moved channels
   - Stop and rescan
   - Try handshake again

4. **Deauth blocked**
   - Some routers ignore deauth frames
   - Protected Management Frames (PMF) enabled
   - Try different network

---

### Issue: Low packet count

**Symptoms:**
- Only a few packets captured
- Expected more packets
- Handshake may be incomplete

**Solutions:**

1. **Scan longer**
   - Wait 30-60 seconds
   - More time = more packets

2. **Closer to router**
   - Move closer to target network
   - Better signal = more packets

3. **Active network**
   - Target network with active clients
   - More clients = more traffic

---

### Issue: Too many packets

**Symptoms:**
- Packet count very high (hundreds)
- File very large
- Memory warnings

**Solutions:**

1. **Stop sooner**
   - Don't need hundreds of packets
   - 50-100 usually sufficient

2. **Targeted capture**
   - Use handshake capture (Left button)
   - More focused than general scan

3. **Clear old data**
   - Delete old capture files
   - Free up SD card space

---

## File and Storage Issues

### Issue: Capture file not saved

**Symptoms:**
- Scanning works
- No file in `/ext/apps_data/flipagotcha/`
- Packet count shows captured packets

**Solutions:**

1. **Check SD card**
   - Ensure SD card inserted
   - Check free space
   - Try different SD card

2. **Verify directory**
   - Check `/ext/apps_data/` exists
   - App should create `flipagotcha/` subdirectory
   - Check permissions

3. **File system errors**
   - Back up SD card
   - Check for errors: Settings → Storage → SD Info
   - Reformat if necessary (FAT32)

---

### Issue: Cannot open capture file

**Symptoms:**
- File exists but can't open
- Corrupted data
- Unreadable format

**Solutions:**

1. **Use text editor**
   - File is plain text
   - Try different text editor
   - Transfer to computer to view

2. **File incomplete**
   - Capture may have failed
   - Delete and recapture

3. **SD card corruption**
   - Run filesystem check
   - Copy important files
   - Reformat SD card

---

### Issue: SD card full

**Symptoms:**
- "Failed to save" error
- Storage warning
- Cannot capture new packets

**Solutions:**

1. **Delete old captures**
   - Remove old `.txt` files
   - Keep only needed captures

2. **Use larger SD card**
   - Upgrade to bigger capacity
   - 16GB+ recommended

3. **Regular cleanup**
   - Download captures to computer
   - Delete from Flipper regularly

---

## Performance Issues

### Issue: App runs slowly

**Symptoms:**
- Laggy interface
- Slow response to buttons
- Face animation stutters

**Solutions:**

1. **Restart app**
   - Exit and relaunch
   - Clears memory

2. **Restart Flipper Zero**
   - Long press Back
   - Select Reboot

3. **Too many packets**
   - Large packet buffer can slow down
   - Stop and save more frequently
   - Clear old captures

---

### Issue: Battery drains quickly

**Symptoms:**
- Flipper battery depletes fast
- Shorter runtime than expected

**Solutions:**

1. **Use external power for ESP32**
   - Don't power ESP32 from Flipper
   - Use separate power source
   - Reduces drain significantly

2. **Shorter scan sessions**
   - Don't scan continuously
   - Stop when done
   - Exit app when not in use

3. **Check battery health**
   - Flipper battery may be aging
   - Replace if necessary

---

### Issue: ESP32 overheating

**Symptoms:**
- ESP32 very warm/hot
- Connection instability
- Unexpected resets

**Solutions:**

1. **Add cooling**
   - Ensure airflow around ESP32
   - Small heatsink helps
   - Don't cover ESP32

2. **External power**
   - Stable power reduces heat
   - Better voltage regulation

3. **Firmware issue**
   - Update Marauder to latest
   - Some versions have power issues

---

## ESP32 Marauder Issues

### Issue: Marauder commands don't work

**Symptoms:**
- ESP32 connected but nothing happens
- No response to commands
- No scanning activity

**Solutions:**

1. **Verify Marauder version**
   - Some commands changed between versions
   - Update to latest Marauder

2. **Test directly**
   - Connect to computer
   - Serial terminal: try `scanap`, `help`
   - If these don't work, Marauder issue

3. **Reflash firmware**
   - Erase flash completely
   - Reflash Marauder
   - Verify after flash

---

### Issue: Wrong Marauder board type

**Symptoms:**
- Marauder loads but doesn't scan
- WiFi not working
- Errors in Marauder

**Solutions:**

1. **Identify your ESP32 board**
   - ESP32-WROOM
   - ESP32-S2
   - ESP32-C3
   - Different variants need different firmware

2. **Download correct firmware**
   - Match board type exactly
   - Check Marauder release notes

3. **Flash correct version**
   - Use proper `.bin` file for your board

---

### Issue: Marauder keeps restarting

**Symptoms:**
- ESP32 boots then restarts
- Boot loop
- Unstable operation

**Solutions:**

1. **Power issue**
   - Insufficient power supply
   - Use quality USB cable
   - Try different power source

2. **Bad flash**
   - Erase and reflash
   - Use slower baud rate for flashing

3. **Hardware defect**
   - Try different ESP32 board
   - Some boards have issues

---

## Getting More Help

### Before Asking for Help

Collect this information:

1. **Flipper Zero Info**
   - Firmware version
   - SD card size and free space

2. **ESP32 Info (if using)**
   - Board type
   - Marauder version
   - How it's powered

3. **Connection Details**
   - Wire connections used
   - Connection indicator ("ESP32" or "DEMO")

4. **Exact Symptoms**
   - What you did
   - What happened
   - Error messages (if any)
   - When problem started

### Support Channels

1. **GitHub Issues**
   - [Open an issue](https://github.com/1090mb/flipagotcha/issues)
   - Include diagnostic info
   - Attach screenshots if possible

2. **Flipper Zero Community**
   - [Flipper Zero Forum](https://forum.flipperzero.one/)
   - Lots of helpful users
   - General Flipper questions

3. **ESP32 Marauder**
   - [Marauder GitHub](https://github.com/justcallmekoko/ESP32Marauder/issues)
   - For Marauder-specific issues
   - Hardware compatibility questions

### Debug Mode

Build with debugging enabled:

```bash
./fbt DEBUG=1 APPSRC=applications_user/flipagotcha
```

Enable logging:
- Connect Flipper via USB
- Open serial console
- Launch app
- Watch log output

### Logs

Check Furi logs for errors:
- Connect to computer
- Serial terminal to Flipper
- Look for `flipagotcha` messages
- Note any errors

---

## Additional Resources

- [User Guide](User-Guide.md) - Complete usage instructions
- [Hardware Setup](Hardware-Setup.md) - ESP32 connection guide
- [FAQ](FAQ.md) - Common questions
- [GitHub Issues](https://github.com/1090mb/flipagotcha/issues) - Known issues

---

**Still Having Problems?**

Open a [GitHub Issue](https://github.com/1090mb/flipagotcha/issues) with:
- Your system information
- Steps to reproduce
- Expected vs actual behavior
- Any error messages
- Screenshots or logs
