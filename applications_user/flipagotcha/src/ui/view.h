#pragma once
#include <gui/view_port.h>
#include <gui/gui.h>
#include <stdbool.h>
#include "../common/protocol.h"  // For PacketFilterType

// Forward declaration
typedef struct WifiScanner WifiScanner;

typedef enum {
    UI_MODE_MAIN,      // Main screen with face animation
    UI_MODE_FILTER     // Filter selection menu
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
} UiState;

// Filter menu configuration
#define FILTER_MENU_ITEM_COUNT 7
extern const char* FILTER_MENU_ITEMS[FILTER_MENU_ITEM_COUNT];
extern const PacketFilterType FILTER_MENU_TYPES[FILTER_MENU_ITEM_COUNT];

void ui_thread_entry(void* args);
