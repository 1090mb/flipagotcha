# Flipagotcha Quick Reference Card

## 🎮 Button Controls

| Button | Action |
|--------|--------|
| **OK** | Start/Stop scanning & capture |
| **←** | Open **Attack Menu** |
| **→** | View **Statistics** |
| **↑** | Open **Packet Filters** |
| **↓** | Open **Settings** |
| **Back** | Exit or return to main |

## 🎭 Face Expressions

| Face | Meaning |
|------|---------|
| **^_^** | Happy - Packets captured! |
| **>_<** | Attacking - Attack in progress |
| **•_•** | Scanning - Looking for networks |
| **x_x** | Confused - ESP32 not connected |
| **•‿•** | Normal - Ready and waiting |
| **-_-** | Sleeping - Not active |

## ⚔️ Attack Menu (←)

1. **Deauth Attack** - Disconnect clients from APs
2. **Beacon Spam** - Inject fake networks
3. **PMKID Capture** - Capture PMKID hashes
4. **Stop Attack** - End current attack
5. **Channel Hop** - Toggle auto channel switching
6. **Back** - Return to main

## 📊 Packet Filters (↑)

- ☑ **Beacon** - Network advertisements
- ☑ **Probe Req** - Client searches
- ☑ **Probe Resp** - AP responses
- ☑ **Data** - Network traffic
- ☑ **Deauth** - Disconnect frames
- ☑ **EAPOL** - Handshake packets

Toggle each with **OK** button

## 📈 Statistics (→)

- **Handshakes**: WPA handshakes captured
- **Deauths**: Deauth frames sent
- **Beacons**: Beacon frames sent
- **PMKIDs**: PMKID hashes captured
- **Packets**: Total packets captured

## 🎯 Quick Workflows

### Scan Networks
```
OK → (scanning) → OK → (saved)
```

### Deauth Attack
```
← → Deauth Attack → OK → (attacking)
← → Stop Attack → OK
```

### PMKID Capture
```
OK → ← → PMKID Capture → OK
→ (check stats)
```

### Channel Hopping
```
← → Channel Hop → OK → Back
```

### Filter Packets
```
↑ → Select type → OK → Back
```

## 🔌 ESP32 Connection

| Indicator | Meaning |
|-----------|---------|
| **ESP32** ▪ | Connected & ready |
| **DEMO** ⊠ | Not connected (mock) |

## 💾 File Storage

Captures saved to:
```
/ext/apps_data/flipagotcha/capture.txt
```

## ⚠️ Status Indicators

| Text | Meaning |
|------|---------|
| `Scanning...` | Actively scanning |
| `Networks: X` | Networks found |
| `ATK: Deauth` | Deauth attack active |
| `ATK: Beacon` | Beacon spam active |
| `ATK: PMKID` | PMKID capture active |
| `Packets: X` | Packets captured |

## 🔧 Menu Navigation

**In any menu:**
- **↑/↓** - Navigate items
- **OK** - Select/toggle item
- **Back** - Return to main screen

**Checkboxes:**
- `[X]` - Enabled
- `[ ]` - Disabled

## 📋 Common Tasks

### Start Session
1. Launch app
2. Check ESP32 connected
3. Press **OK** to scan
4. Wait for networks

### Run Deauth
1. Scan for networks (OK)
2. Open Attack Menu (←)
3. Select Deauth Attack
4. Press OK to start
5. Check face shows >_<

### Check Progress
- Press **→** for statistics
- Look at packet count
- Check face expression

### Save & Exit
1. Press **OK** to stop scan
2. Captures auto-saved
3. Press **Back** to exit

## 💡 Pro Tips

- **Scan first** before attacking
- **Check stats** frequently (→)
- **Use filters** to save space (↑)
- **Enable channel hop** for coverage
- **Stop attacks** when done
- **Save often** - auto-save not yet implemented

## 🚨 Troubleshooting

| Problem | Solution |
|---------|----------|
| Face shows x_x | Check ESP32 connection |
| No networks found | Enable channel hopping |
| No packets captured | Check packet filters |
| Attack not working | Verify DEMO mode off |
| Can't see face | Screen brightness? |

## 📖 More Info

- **Full Guide**: [BETTERCAP_FEATURES.md](BETTERCAP_FEATURES.md)
- **UI Layout**: [UI_GUIDE.md](UI_GUIDE.md)
- **WiFi Scanner**: [WIFI_SCANNER.md](WIFI_SCANNER.md)
- **ESP32 Setup**: [ESP32_SETUP.md](ESP32_SETUP.md)

## ⚖️ Legal Notice

**FOR AUTHORIZED TESTING ONLY**

Only use on networks you own or have explicit written permission to test. Unauthorized WiFi attacks are illegal.

---

**Quick Start**: Connect ESP32 → Launch app → Press OK → Start attacking!

**Support**: Visit [Issues](https://github.com/1090mb/flipagotcha/issues)

---

*Print this card for quick reference while using Flipagotcha!*
