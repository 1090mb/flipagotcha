# Packet Filtering Feature

## Overview
This feature adds the ability to filter specific WiFi packet types during capture. Users can select which packet types to capture through an interactive menu interface.

## Changes Made

### 1. Protocol Extensions (`src/common/protocol.h`)
- Added `PacketFilterType` enum with filter flags for:
  - Beacon frames
  - Probe request/response frames
  - Data frames
  - Deauthentication frames
  - EAPOL/handshake frames
- Added `PacketFilterConfig` structure to store filter settings

### 2. WiFi Scanner Module (`src/hw/wifi_scanner.[ch]`)
- Added `filter_config` field to `WifiScanner` structure
- Implemented filter management functions:
  - `wifi_scanner_set_filter()` - Set filter configuration
  - `wifi_scanner_get_filter()` - Get current filter configuration
  - `wifi_scanner_toggle_filter()` - Toggle individual filter types
- Enhanced `wifi_scanner_start_capture()` to use filter configuration:
  - Uses optimized `sniffpmkid` command when only EAPOL filter is enabled
  - Uses `sniffraw` for other filter combinations
- Initialized default filter to capture all packet types (`PACKET_FILTER_ALL`)

### 3. User Interface (`src/ui/view.[ch]`)
- Added `UI_MODE_FILTER` enum value for filter menu mode
- Added `filter_selection` field to track current menu selection
- Implemented `draw_filter_menu()` function to display filter options with checkboxes
- Updated `input_callback()` to handle filter menu navigation:
  - Up/Down arrows: Navigate menu options
  - OK button: Toggle selected filter or return to main screen
  - Back button: Return to main screen
- Modified main screen Up arrow key to open filter menu (was smile)

### 4. Documentation (`WIFI_SCANNER.md`)
- Updated button controls section with filter menu instructions
- Added packet filter options section
- Added packet filtering implementation section with API documentation
- Updated data structures section with new filter types

## Usage

### Main Screen
1. Press **Up arrow** to open the packet filter menu
2. Press **OK button** to start scanning and capture with current filters

### Filter Menu
1. Use **Up/Down arrows** to navigate filter options
2. Press **OK button** to toggle the selected filter on/off
3. Press **OK button** on "Back" option or **Back button** to return to main screen

### Filter Options
- **Beacon**: Captures beacon frames from access points
- **Probe Req**: Captures probe request frames from devices
- **Probe Resp**: Captures probe response frames from access points
- **Data**: Captures data frames
- **Deauth**: Captures deauthentication frames
- **EAPOL**: Captures EAPOL/handshake frames (WPA/WPA2)

Multiple filters can be enabled simultaneously. Checkboxes show current state:
- `[X]` = Filter enabled (packets will be captured)
- `[ ]` = Filter disabled (packets will be ignored)

## Building

Build with the standard Flipper Zero build process:
```bash
./fbt COMPACT=1 APPSRC=applications_user/flipagotcha
```

## Testing

Testing requires Flipper Zero hardware with ESP32 Marauder module connected:
1. Flash the built application to Flipper Zero
2. Connect ESP32 Marauder via GPIO pins (see ESP32_SETUP.md)
3. Launch the Flipagotcha application
4. Test filter menu navigation with Up/Down/OK buttons
5. Enable/disable different filter combinations
6. Start scan and capture to verify correct packet types are captured
7. Review saved capture file to confirm filtering worked

## Integration with ensure-app-redundancy Branch

The packet filtering feature has been implemented on the `copilot/add-packet-filtering-option` branch. To integrate with the `ensure-app-redundancy` branch:

### Option 1: Create Branch and Merge (Manual)
```bash
# Create ensure-app-redundancy branch from current state
git checkout -b ensure-app-redundancy

# Push the new branch
git push -u origin ensure-app-redundancy
```

### Option 2: Merge via Pull Request
1. Create a pull request from `copilot/add-packet-filtering-option` to a new `ensure-app-redundancy` branch
2. Review and approve the changes
3. Merge the pull request

## Future Enhancements

Possible improvements for this feature:
1. Add real-time packet filtering during parsing (currently filters are applied at capture start)
2. Add preset filter configurations (e.g., "Handshake Only", "Management Only")
3. Add packet count per filter type in the UI
4. Add ability to save/load filter configurations
5. Add more granular filter types (e.g., specific management frame subtypes)
