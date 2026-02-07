# Flipagotcha UI Guide

This guide shows the visual layout and navigation of the Flipagotcha user interface.

## Main Screen

```
┌────────────────────────────────┐
│ Scanning...            ESP32 ▪ │  ← Status bar
│ Networks: 5                    │
│                                │
│           •‿•                  │  ← Dynamic face
│           ^^^                  │
│                                │
│ ATK: Deauth                    │  ← Attack indicator
│                                │
│                                │
│ Packets: 145                   │
└────────────────────────────────┘
     ↑    OK    ↑    
    Left      Right
   Attack     Stats
```

### Navigation from Main Screen
- **OK Button**: Start/Stop scanning
- **Left Arrow** → Attack Menu
- **Right Arrow** → Statistics View  
- **Up Arrow** → Packet Filter Menu
- **Down Arrow** → Settings Menu
- **Back Button**: Exit app

---

## Face Expressions

### Normal (Ready)
```
  •‿•
  ^^^
```
State: Idle, ready to scan

### Happy (Captures!)
```
  ^_^
  ╰─╯
```
State: Successfully captured packets

### Attacking (Active)
```
  >_<
  ^^^
```
State: Attack in progress

### Scanning (Focus)
```
  •_•
  ───
```
State: Scanning for networks

### Confused (No ESP32)
```
  x_x
  ╰─╯
```
State: ESP32 not connected (DEMO mode)

### Sleeping (Idle)
```
  -_-
  ───
```
State: Not initialized

---

## Attack Menu (Left Arrow)

```
┌────────────────────────────────┐
│ Attack Menu                    │
│                                │
│ > Deauth Attack                │  ← Selected
│   Beacon Spam                  │
│   PMKID Capture                │
│   Stop Attack                  │
│   Channel Hop        [ ]       │  ← Toggle
│   Back                         │
│                                │
│ OK:Select Up/Dn:Nav            │
└────────────────────────────────┘
```

### Menu Options
- **Deauth Attack**: Send deauth frames to disconnect clients
- **Beacon Spam**: Inject fake beacon frames
- **PMKID Capture**: Capture PMKID hashes
- **Stop Attack**: Stop any active attack
- **Channel Hop**: Toggle automatic channel hopping
- **Back**: Return to main screen

### Controls
- **Up/Down**: Navigate menu
- **OK**: Select option
- **Back**: Return to main screen

---

## Statistics View (Right Arrow)

```
┌────────────────────────────────┐
│ Statistics                     │
│                                │
│ Handshakes: 3                  │
│ Deauths: 150                   │
│ Beacons: 500                   │
│ PMKIDs: 2                      │
│ Packets: 1234                  │
│                                │
│                                │
│ Back:Return                    │
└────────────────────────────────┘
```

### Statistics Tracked
- **Handshakes**: WPA/WPA2 handshakes captured
- **Deauths**: Deauthentication frames sent
- **Beacons**: Beacon frames injected  
- **PMKIDs**: PMKID hashes captured
- **Packets**: Total packets captured

### Controls
- **OK** or **Back**: Return to main screen

---

## Packet Filter Menu (Up Arrow)

```
┌────────────────────────────────┐
│ Packet Filters                 │
│                                │
│ > Beacon           [X]         │  ← Selected & enabled
│   Probe Req        [X]         │
│   Probe Resp       [X]         │
│   Data             [X]         │
│   Deauth           [X]         │
│   EAPOL            [X]         │
│   Back                         │
│                                │
│ OK:Toggle Up/Dn:Nav            │
└────────────────────────────────┘
```

### Filter Types
- **Beacon**: Beacon frames (network advertisements)
- **Probe Req**: Probe request frames
- **Probe Resp**: Probe response frames
- **Data**: Data frames
- **Deauth**: Deauthentication frames
- **EAPOL**: EAPOL frames (handshakes)

### Controls
- **Up/Down**: Navigate filters
- **OK**: Toggle selected filter on/off
- **Back**: Return to main screen

---

## Settings Menu (Down Arrow)

```
┌────────────────────────────────┐
│ Settings                       │
│                                │
│ > Auto Save: OFF               │  ← Selected
│   Save Interval: 5m            │
│   Audio: OFF                   │
│   Channel: Auto                │
│   Back                         │
│                                │
│                                │
│ OK:Toggle Bk:Back              │
└────────────────────────────────┘
```

### Settings Options
- **Auto Save**: Automatic capture file saving
- **Save Interval**: How often to auto-save captures
- **Audio**: Sound feedback for captures
- **Channel**: Manual channel selection or auto
- **Back**: Return to main screen

### Controls
- **Up/Down**: Navigate settings
- **OK**: Toggle/change setting
- **Back**: Return to main screen

---

## Status Indicators

### ESP32 Connection
```
ESP32 ▪   ← Connected (filled square)
DEMO  ⊠   ← Not connected (X'd square)
```

### Attack Status
```
ATK: Deauth    ← Deauth attack active
ATK: Beacon    ← Beacon spam active
ATK: PMKID     ← PMKID capture active
(none)         ← No attack running
```

---

## User Workflows

### 1. Basic Network Scan
```
Main Screen
    ↓ Press OK
Scanning... (shows network count)
    ↓ Press OK again
Stops scanning, saves capture
```

### 2. Deauth Attack
```
Main Screen
    ↓ Left Arrow
Attack Menu
    ↓ Select "Deauth Attack"
    ↓ Press OK
Returns to Main (shows ATK: Deauth)
    ↓ Left Arrow → "Stop Attack"
Attack stops
```

### 3. PMKID Capture
```
Main Screen
    ↓ Press OK (start scan)
Wait for networks...
    ↓ Left Arrow
Attack Menu
    ↓ Select "PMKID Capture"
    ↓ Press OK
Returns to Main (face shows attacking)
    ↓ Wait for captures...
    ↓ Right Arrow
View statistics (PMKIDs: X)
```

### 4. Channel Hopping
```
Main Screen
    ↓ Left Arrow
Attack Menu
    ↓ Select "Channel Hop"
    ↓ Press OK (toggle on)
Checkbox shows [X]
    ↓ Back
Continues hopping on main screen
```

### 5. Filter Packets
```
Main Screen
    ↓ Up Arrow
Filter Menu
    ↓ Navigate to packet type
    ↓ Press OK to toggle
Checkbox updates [X] or [ ]
    ↓ Back
Only selected types captured
```

### 6. View Statistics
```
Main Screen
    ↓ Right Arrow
Statistics View
    (View capture metrics)
    ↓ Back
Returns to Main
```

---

## Screen States

### Idle (Not Scanning)
```
┌────────────────────────────────┐
│                        ESP32 ▪ │
│                                │
│                                │
│           •‿•                  │
│           ^^^                  │
│                                │
│                                │
│                                │
│                                │
│                                │
└────────────────────────────────┘
```

### Scanning Active
```
┌────────────────────────────────┐
│ Scanning...            ESP32 ▪ │
│ Networks: 8                    │
│                                │
│           •_•                  │
│           ───                  │
│                                │
│                                │
│                                │
│ Packets: 42                    │
└────────────────────────────────┘
```

### Attack Active
```
┌────────────────────────────────┐
│ Scanning...            ESP32 ▪ │
│ Networks: 8                    │
│                                │
│           >_<                  │
│           ^^^                  │
│ ATK: Deauth                    │
│                                │
│                                │
│ Packets: 89                    │
└────────────────────────────────┘
```

### No ESP32 (DEMO Mode)
```
┌────────────────────────────────┐
│                         DEMO ⊠ │
│                                │
│                                │
│           x_x                  │
│           ╰─╯                  │
│                                │
│                                │
│                                │
│                                │
└────────────────────────────────┘
```

### Successful Captures
```
┌────────────────────────────────┐
│                        ESP32 ▪ │
│                                │
│                                │
│           ^_^                  │
│           ╰─╯                  │
│                                │
│                                │
│                                │
│ Packets: 256                   │
└────────────────────────────────┘
```

---

## Tips

### Quick Actions
- **Double-tap OK**: Quick start/stop scanning
- **Hold Back**: Exit app immediately
- **Left → OK**: Quick deauth attack
- **Right**: Quick stats check

### Visual Cues
- **Face expression**: Current state at a glance
- **ATK indicator**: Know when attack is running
- **ESP32/DEMO**: Connection status
- **Packet count**: Capture progress

### Best Practices
1. Always scan (OK) before attacking
2. Check stats (Right) regularly
3. Use filters (Up) to save storage
4. Stop attacks when done
5. Save captures before exiting

---

## Screen Dimensions

Flipper Zero Display: **128 × 64 pixels**

### Layout Zones
```
┌─────────────────────128px──────────────────┐
│ Status Text [0-100px]  │  ESP32 [90-128px] │ 0-12px
│────────────────────────┴────────────────────│
│                                             │
│              Face Area                      │ 12-48px
│           (centered ~40-60px)               │
│                                             │
│────────────────────────────────────────────│
│ Attack Status [0-100px]                    │ 48-56px
│────────────────────────────────────────────│
│ Packet Count [0-100px]                     │ 56-64px
└─────────────────────────────────────────────┘
```

### Menu Layout
```
┌─────────────────────128px──────────────────┐
│ Menu Title                                  │ 0-12px
│─────────────────────────────────────────────│
│ > Item 1                         [X]        │ 20px
│   Item 2                         [ ]        │ 28px
│   Item 3                         [X]        │ 36px
│   Item 4                         [ ]        │ 44px
│   Item 5                         [X]        │ 52px
│─────────────────────────────────────────────│
│ Help Text                                   │ 60-64px
└─────────────────────────────────────────────┘
```

---

## Keyboard Shortcuts Summary

| Button | Main Screen | Menu Screens |
|--------|-------------|--------------|
| OK | Start/Stop Scan | Select Item |
| Left | Attack Menu | — |
| Right | Statistics | — |
| Up | Filter Menu | Navigate Up |
| Down | Settings Menu | Navigate Down |
| Back | Exit App | Return to Main |

---

## Color Scheme

Flipper Zero is **monochrome** (black and white):
- **Black**: Active pixels (text, icons, face)
- **White**: Background
- **Inverted**: Not used (to preserve battery)

All UI elements use simple line drawing and text rendering.

---

## Accessibility

### Visual Indicators
- Large, clear text (FontSecondary: 8px, FontPrimary: 10px)
- High contrast black on white
- Simple geometric face (easy to read)
- Status icons (filled vs outlined)

### Navigation
- Linear menu navigation
- Consistent button mappings
- Always have "Back" option
- Help text on every menu screen

### Feedback
- Face changes with state
- Attack indicator text
- Packet count updates
- Statistics view for confirmation

---

## Future UI Enhancements

Planned improvements:
- [ ] Progress bars for long operations
- [ ] Animation during attacks (face)
- [ ] Scrolling text for long SSIDs
- [ ] Graph view for signal strength
- [ ] Mini-map of channels
- [ ] Battery indicator integration
- [ ] Time elapsed counter
- [ ] Capture rate meter

---

*For more information, see [BETTERCAP_FEATURES.md](BETTERCAP_FEATURES.md)*
