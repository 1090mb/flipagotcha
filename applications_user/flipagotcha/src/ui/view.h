#pragma once
#include <gui/view_port.h>
#include <gui/gui.h>
#include <stdbool.h>
#include "../common/protocol.h"  // For PacketFilterType

// Forward declaration
typedef struct WifiScanner WifiScanner;

typedef enum {
    UI_MODE_MAIN,      // Main screen with face animation
    UI_MODE_FILTER,    // Filter selection menu
    UI_MODE_ATTACK,    // Attack menu (deauth, beacon spam, etc.)
    UI_MODE_SETTINGS,  // Settings menu
    UI_MODE_STATS      // Statistics view
} UiMode;

typedef struct {
    ViewPort* vp;
    Gui* gui;
    bool eyes_closed;
    bool mouth_frown;
    bool scanning;
    uint8_t current_channel;
    FuriTimer* anim_timer;
    WifiScanner* wifi_scanner;
    uint8_t network_count;
    uint16_t packet_count;
    UiMode mode;              // Current UI mode
    uint8_t filter_selection; // Current filter menu selection (0-6)
    uint8_t attack_selection; // Current attack menu selection
    uint8_t settings_selection; // Current settings menu selection
    AttackType active_attack;  // Currently active attack type
    bool channel_hopping;     // Channel hopping enabled
    // Cached formatted strings to avoid repeated snprintf in draw callback
    char network_count_str[32];
    char packet_count_str[32];
    uint8_t cached_network_count;
    uint16_t cached_packet_count;
} UiState;

// Filter menu configuration
#define FILTER_MENU_ITEM_COUNT 7
extern const char* FILTER_MENU_ITEMS[FILTER_MENU_ITEM_COUNT];
extern const PacketFilterType FILTER_MENU_TYPES[FILTER_MENU_ITEM_COUNT];

// Attack menu configuration
#define ATTACK_MENU_ITEM_COUNT 6
extern const char* ATTACK_MENU_ITEMS[ATTACK_MENU_ITEM_COUNT];

// Settings menu configuration
#define SETTINGS_MENU_ITEM_COUNT 5
extern const char* SETTINGS_MENU_ITEMS[SETTINGS_MENU_ITEM_COUNT];

void ui_thread_entry(void* args);
