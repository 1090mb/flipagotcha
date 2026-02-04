#pragma once
#include <gui/view_port.h>
#include <gui/gui.h>
#include <stdbool.h>

typedef struct {
    ViewPort* vp;
    Gui* gui;
    bool eyes_closed;
    bool mouth_frown;
    bool scanning;
    uint8_t current_channel;
    FuriTimer* anim_timer;
} UiState;

void ui_thread_entry(void* args);
