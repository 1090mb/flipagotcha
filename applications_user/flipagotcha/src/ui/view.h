#pragma once
#include <gui/view_port.h>
#include <gui/gui.h>
#include <stdbool.h>

// Forward declaration
typedef struct WifiScanner WifiScanner;

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
} UiState;

void ui_thread_entry(void* args);
