#include "view.h"
#include "draw.h"
#include "../hw/uart.h"
#include "../hw/wifi_scanner.h"
#include "../common/protocol.h"
#include <furi.h>
#include <input/input.h>
#include <stdlib.h>
#include <string.h>

#define ANIM_TICK_MS 100

// Filter menu configuration - extracted to avoid duplication
const char* FILTER_MENU_ITEMS[FILTER_MENU_ITEM_COUNT] = {
    "Beacon",
    "Probe Req",
    "Probe Resp",
    "Data",
    "Deauth",
    "EAPOL",
    "Back"
};

const PacketFilterType FILTER_MENU_TYPES[FILTER_MENU_ITEM_COUNT] = {
    PACKET_FILTER_BEACON,
    PACKET_FILTER_PROBE_REQ,
    PACKET_FILTER_PROBE_RESP,
    PACKET_FILTER_DATA,
    PACKET_FILTER_DEAUTH,
    PACKET_FILTER_EAPOL,
    0  // Back option
};

// Attack menu configuration
const char* ATTACK_MENU_ITEMS[ATTACK_MENU_ITEM_COUNT] = {
    "Deauth Attack",
    "Beacon Spam",
    "PMKID Capture",
    "Stop Attack",
    "Channel Hop",
    "Back"
};

// Settings menu configuration
const char* SETTINGS_MENU_ITEMS[SETTINGS_MENU_ITEM_COUNT] = {
    "Auto Save: OFF",
    "Save Interval: 5m",
    "Audio: OFF",
    "Channel: Auto",
    "Back"
};

/* ------------------------------------------------------------------ */
static void draw_main_screen(Canvas* canvas, UiState* st) {
    /* Determine face expression based on state */
    FaceExpression expression = FACE_NORMAL;
    
    if (st->wifi_scanner) {
        bool esp32_connected = wifi_scanner_is_esp32_connected(st->wifi_scanner);
        
        if (!esp32_connected) {
            expression = FACE_CONFUSED;  // No ESP32
        } else if (st->active_attack != ATTACK_TYPE_NONE) {
            expression = FACE_ATTACKING;  // Currently attacking
        } else if (st->scanning) {
            expression = FACE_SCANNING;  // Scanning for networks
        } else if (st->packet_count > 0) {
            expression = FACE_HAPPY;  // Has captured packets
        } else {
            expression = FACE_NORMAL;  // Idle and ready
        }
    } else {
        expression = FACE_SLEEPING;  // No scanner
    }
    
    /* Draw the face with appropriate expression */
    draw_face_expression(canvas, 40, 20, expression);
    
    /* Draw WiFi status */
    canvas_set_font(canvas, FontSecondary);
    
    /* Show ESP32 connection status */
    if (st->wifi_scanner) {
        bool esp32_connected = wifi_scanner_is_esp32_connected(st->wifi_scanner);
        canvas_draw_str(canvas, 90, 10, esp32_connected ? "ESP32" : "DEMO");

        /* Small monochrome status icon (6x6) to reinforce connection state.
         * - Connected: filled square
         * - Not connected: outlined square with X
         * Fits official 128x64 monochrome LCD.
         */
        int icon_x = 114;
        int icon_y = 8;
        if (esp32_connected) {
            for (int xx = icon_x; xx < icon_x + 6; xx++) {
                for (int yy = icon_y; yy < icon_y + 6; yy++) {
                    canvas_draw_point(canvas, xx, yy);
                }
            }
        } else {
            // Outline
            canvas_draw_line(canvas, icon_x, icon_y, icon_x + 5, icon_y);
            canvas_draw_line(canvas, icon_x, icon_y + 5, icon_x + 5, icon_y + 5);
            canvas_draw_line(canvas, icon_x, icon_y, icon_x, icon_y + 5);
            canvas_draw_line(canvas, icon_x + 5, icon_y, icon_x + 5, icon_y + 5);
            // X
            canvas_draw_line(canvas, icon_x, icon_y, icon_x + 5, icon_y + 5);
            canvas_draw_line(canvas, icon_x + 5, icon_y, icon_x, icon_y + 5);
        }
    }
    
    if (st->scanning) {
        canvas_draw_str(canvas, 2, 10, "Scanning...");
        
        // Use cached string if count hasn't changed
        if (st->cached_network_count != st->network_count) {
            snprintf(st->network_count_str, sizeof(st->network_count_str), "Networks: %u", st->network_count);
            st->cached_network_count = st->network_count;
        }
        canvas_draw_str(canvas, 2, 20, st->network_count_str);
    }
    
    // Show active attack status
    if (st->active_attack != ATTACK_TYPE_NONE) {
        const char* attack_name = "";
        switch (st->active_attack) {
            case ATTACK_TYPE_DEAUTH:
                attack_name = "Deauth";
                break;
            case ATTACK_TYPE_BEACON_SPAM:
                attack_name = "Beacon";
                break;
            case ATTACK_TYPE_PMKID:
                attack_name = "PMKID";
                break;
            default:
                break;
        }
        char attack_str[32];
        snprintf(attack_str, sizeof(attack_str), "ATK: %s", attack_name);
        canvas_draw_str(canvas, 2, 30, attack_str);
    }
    
    if (st->packet_count > 0) {
        // Use cached string if count hasn't changed
        if (st->cached_packet_count != st->packet_count) {
            snprintf(st->packet_count_str, sizeof(st->packet_count_str), "Packets: %u", st->packet_count);
            st->cached_packet_count = st->packet_count;
        }
        canvas_draw_str(canvas, 2, 60, st->packet_count_str);
    }
}

static void draw_filter_menu(Canvas* canvas, UiState* st) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Packet Filters");
    
    canvas_set_font(canvas, FontSecondary);
    
    // Get current filter configuration
    uint8_t filter = wifi_scanner_get_filter(st->wifi_scanner);
    
    // Draw menu items
    for (uint8_t i = 0; i < FILTER_MENU_ITEM_COUNT; i++) {
        int y = 20 + (i * 8);
        
        // Highlight selected item
        if (i == st->filter_selection) {
            canvas_draw_str(canvas, 2, y, ">");
        }
        
        canvas_draw_str(canvas, 10, y, FILTER_MENU_ITEMS[i]);
        
        // Show checkbox for filter items (not for "Back" option)
        if (i < FILTER_MENU_ITEM_COUNT - 1) {
            bool enabled = (filter & FILTER_MENU_TYPES[i]) != 0;
            canvas_draw_str(canvas, 80, y, enabled ? "[X]" : "[ ]");
        }
    }
    
    // Help text
    canvas_draw_str(canvas, 2, 60, "OK:Toggle Up/Dn:Nav");
}

static void draw_attack_menu(Canvas* canvas, UiState* st) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Attack Menu");
    
    canvas_set_font(canvas, FontSecondary);
    
    // Draw menu items
    for (uint8_t i = 0; i < ATTACK_MENU_ITEM_COUNT; i++) {
        int y = 20 + (i * 8);
        
        // Highlight selected item
        if (i == st->attack_selection) {
            canvas_draw_str(canvas, 2, y, ">");
        }
        
        canvas_draw_str(canvas, 10, y, ATTACK_MENU_ITEMS[i]);
        
        // Show indicator for channel hop toggle
        if (i == 4) { // Channel Hop option
            canvas_draw_str(canvas, 90, y, st->channel_hopping ? "[X]" : "[ ]");
        }
    }
    
    // Help text
    canvas_draw_str(canvas, 2, 60, "OK:Select Up/Dn:Nav");
}

static void draw_settings_menu(Canvas* canvas, UiState* st) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Settings");
    
    canvas_set_font(canvas, FontSecondary);
    
    // Draw menu items
    for (uint8_t i = 0; i < SETTINGS_MENU_ITEM_COUNT; i++) {
        int y = 20 + (i * 8);
        
        // Highlight selected item
        if (i == st->settings_selection) {
            canvas_draw_str(canvas, 2, y, ">");
        }
        
        canvas_draw_str(canvas, 10, y, SETTINGS_MENU_ITEMS[i]);
    }
    
    // Help text
    canvas_draw_str(canvas, 2, 60, "OK:Toggle Bk:Back");
}

static void draw_stats_view(Canvas* canvas, UiState* st) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Statistics");
    
    canvas_set_font(canvas, FontSecondary);
    
    // Get session statistics
    SessionStats stats = wifi_scanner_get_stats(st->wifi_scanner);
    
    char buf[64];
    
    snprintf(buf, sizeof(buf), "Handshakes: %lu", stats.handshakes_captured);
    canvas_draw_str(canvas, 2, 20, buf);
    
    snprintf(buf, sizeof(buf), "Deauths: %lu", stats.deauth_sent);
    canvas_draw_str(canvas, 2, 28, buf);
    
    snprintf(buf, sizeof(buf), "Beacons: %lu", stats.beacons_sent);
    canvas_draw_str(canvas, 2, 36, buf);
    
    snprintf(buf, sizeof(buf), "PMKIDs: %lu", stats.pmkids_captured);
    canvas_draw_str(canvas, 2, 44, buf);
    
    snprintf(buf, sizeof(buf), "Packets: %lu", stats.total_packets);
    canvas_draw_str(canvas, 2, 52, buf);
    
    // Help text
    canvas_draw_str(canvas, 2, 60, "Back:Return");
}

static void draw_callback(Canvas* canvas, void* ctx) {
    UiState* st = ctx;

    /* Clear background (optional) */
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0, 128, 64);
    canvas_set_color(canvas, ColorWhite);

    switch (st->mode) {
        case UI_MODE_FILTER:
            draw_filter_menu(canvas, st);
            break;
        case UI_MODE_ATTACK:
            draw_attack_menu(canvas, st);
            break;
        case UI_MODE_SETTINGS:
            draw_settings_menu(canvas, st);
            break;
        case UI_MODE_STATS:
            draw_stats_view(canvas, st);
            break;
        case UI_MODE_MAIN:
        default:
            draw_main_screen(canvas, st);
            break;
    }
}

/* ------------------------------------------------------------------ */
static void input_callback(InputEvent* ev, void* ctx) {
    UiState* st = ctx;
    if (ev->type != InputTypeShort) return;

    // Handle filter menu mode
    if (st->mode == UI_MODE_FILTER) {
        switch (ev->key) {
            case InputKeyUp:
                if (st->filter_selection > 0) {
                    st->filter_selection--;
                }
                break;
            case InputKeyDown:
                if (st->filter_selection < FILTER_MENU_ITEM_COUNT - 1) {
                    st->filter_selection++;
                }
                break;
            case InputKeyOk:
                {
                    // Toggle filter or go back
                    if (st->filter_selection == FILTER_MENU_ITEM_COUNT - 1) {
                        // Back option selected
                        st->mode = UI_MODE_MAIN;
                    } else {
                        // Toggle the selected filter
                        wifi_scanner_toggle_filter(st->wifi_scanner, FILTER_MENU_TYPES[st->filter_selection]);
                    }
                }
                break;
            case InputKeyBack:
                st->mode = UI_MODE_MAIN;
                break;
            default:
                break;
        }
        view_port_update(st->vp);
        return;
    }

    // Handle attack menu mode
    if (st->mode == UI_MODE_ATTACK) {
        switch (ev->key) {
            case InputKeyUp:
                if (st->attack_selection > 0) {
                    st->attack_selection--;
                }
                break;
            case InputKeyDown:
                if (st->attack_selection < ATTACK_MENU_ITEM_COUNT - 1) {
                    st->attack_selection++;
                }
                break;
            case InputKeyOk:
                {
                    switch (st->attack_selection) {
                        case 0: // Deauth Attack
                            if (st->network_count > 0) {
                                wifi_scanner_start_deauth(st->wifi_scanner, 0);
                                st->active_attack = ATTACK_TYPE_DEAUTH;
                            }
                            break;
                        case 1: // Beacon Spam
                            wifi_scanner_start_beacon_spam(st->wifi_scanner);
                            st->active_attack = ATTACK_TYPE_BEACON_SPAM;
                            break;
                        case 2: // PMKID Capture
                            wifi_scanner_start_pmkid_capture(st->wifi_scanner);
                            st->active_attack = ATTACK_TYPE_PMKID;
                            break;
                        case 3: // Stop Attack
                            if (st->active_attack == ATTACK_TYPE_DEAUTH) {
                                wifi_scanner_stop_deauth(st->wifi_scanner);
                            } else if (st->active_attack == ATTACK_TYPE_BEACON_SPAM) {
                                wifi_scanner_stop_beacon_spam(st->wifi_scanner);
                            } else if (st->active_attack == ATTACK_TYPE_PMKID) {
                                wifi_scanner_stop_pmkid_capture(st->wifi_scanner);
                            }
                            st->active_attack = ATTACK_TYPE_NONE;
                            break;
                        case 4: // Channel Hop
                            st->channel_hopping = !st->channel_hopping;
                            wifi_scanner_set_channel_hopping(st->wifi_scanner, st->channel_hopping);
                            break;
                        case 5: // Back
                            st->mode = UI_MODE_MAIN;
                            break;
                    }
                }
                break;
            case InputKeyBack:
                st->mode = UI_MODE_MAIN;
                break;
            default:
                break;
        }
        view_port_update(st->vp);
        return;
    }

    // Handle settings menu mode
    if (st->mode == UI_MODE_SETTINGS) {
        switch (ev->key) {
            case InputKeyUp:
                if (st->settings_selection > 0) {
                    st->settings_selection--;
                }
                break;
            case InputKeyDown:
                if (st->settings_selection < SETTINGS_MENU_ITEM_COUNT - 1) {
                    st->settings_selection++;
                }
                break;
            case InputKeyOk:
                {
                    if (st->settings_selection == SETTINGS_MENU_ITEM_COUNT - 1) {
                        // Back option
                        st->mode = UI_MODE_MAIN;
                    }
                    // TODO: Implement settings toggles
                }
                break;
            case InputKeyBack:
                st->mode = UI_MODE_MAIN;
                break;
            default:
                break;
        }
        view_port_update(st->vp);
        return;
    }

    // Handle stats view mode
    if (st->mode == UI_MODE_STATS) {
        switch (ev->key) {
            case InputKeyBack:
            case InputKeyOk:
                st->mode = UI_MODE_MAIN;
                break;
            default:
                break;
        }
        view_port_update(st->vp);
        return;
    }

    // Handle main screen mode
    switch (ev->key) {
        case InputKeyOk:
            {
                // Start WiFi scan
                if (!st->scanning && st->wifi_scanner) {
                    wifi_scanner_start_scan(st->wifi_scanner);
                    st->scanning = true;
                    // Note: In production, network count would be updated asynchronously as networks are discovered
                    // For now, mock data is populated synchronously in wifi_scanner_start_scan()
                    st->network_count = wifi_scanner_get_network_count(st->wifi_scanner);
                    
                    // Also start packet capture
                    wifi_scanner_start_capture(st->wifi_scanner);
                    // Packet count will be updated in the main polling loop
                } else if (st->scanning && st->wifi_scanner) {
                    // Stop scanning and capture
                    wifi_scanner_stop_scan(st->wifi_scanner);
                    wifi_scanner_stop_capture(st->wifi_scanner);
                    st->scanning = false;
                    
                    // Save captured packets
                    wifi_scanner_save_capture(st->wifi_scanner, "capture.txt");
                }
                
                // Also send toggle scan command via UART for ESP32
                uint8_t cmd = CMD_TOGGLE_SCAN;
                uart_write(&cmd, 1);
            }
            break;
        case InputKeyLeft:
            // Open attack menu
            st->mode = UI_MODE_ATTACK;
            st->attack_selection = 0;
            break;
        case InputKeyRight:
            // Open statistics view
            st->mode = UI_MODE_STATS;
            break;
        case InputKeyUp:
            // Open filter menu
            st->mode = UI_MODE_FILTER;
            st->filter_selection = 0;
            break;
        case InputKeyDown:
            // Open settings menu
            st->mode = UI_MODE_SETTINGS;
            st->settings_selection = 0;
            break;
        case InputKeyBack:
            furi_thread_exit(furi_thread_get_current());
            break;
        default:
            break;
    }
    view_port_update(st->vp);
}

/* ------------------------------------------------------------------ */
static void anim_tick(void* ctx) {
    UiState* st = ctx;
    static uint8_t blink_ctr = 0;
    blink_ctr++;
    if (blink_ctr >= 4) { // toggle every 4 ticks ≈400 ms
        st->eyes_closed = !st->eyes_closed;
        blink_ctr = 0;
    }
    view_port_update(st->vp);
}

/* ------------------------------------------------------------------ */
void ui_thread_entry(void* args) {
    (void)args;

    UiState* st = malloc(sizeof(UiState));
    memset(st, 0, sizeof(UiState));
    
    // Initialize UI mode to main screen
    st->mode = UI_MODE_MAIN;
    st->filter_selection = 0;
    st->attack_selection = 0;
    st->settings_selection = 0;
    st->active_attack = ATTACK_TYPE_NONE;
    st->channel_hopping = false;

    /* Allocate WiFi scanner */
    st->wifi_scanner = wifi_scanner_alloc();

    /* ViewPort creation */
    st->vp = view_port_alloc();
    view_port_draw_callback_set(st->vp, draw_callback, st);
    view_port_input_callback_set(st->vp, input_callback, st);

    /* Attach to global GUI */
    st->gui = furi_record_open("gui");
    gui_add_view_port(st->gui, st->vp, GuiLayerFullscreen);

    /* Animation timer */
    st->anim_timer = furi_timer_alloc(anim_tick, FuriTimerTypePeriodic, st);
    furi_timer_start(st->anim_timer, furi_ms_to_ticks(ANIM_TICK_MS));

    /* Keep thread alive until the user exits */
    while (!furi_thread_is_stopped(furi_thread_get_current())) {
        furi_delay_ms(200);

        static uint8_t refresh_ctr = 0;
        refresh_ctr++;
        if (refresh_ctr >= 10) { // ~2 seconds
            refresh_ctr = 0;
            if (st->wifi_scanner) {
                wifi_scanner_refresh_connection(st->wifi_scanner);
            }
        }

        /* Update network and packet counts if scanning */
        if (st->scanning && st->wifi_scanner) {
            st->network_count = wifi_scanner_get_network_count(st->wifi_scanner);
            st->packet_count = wifi_scanner_get_packet_count(st->wifi_scanner);
            view_port_update(st->vp);
        }
        
        /* Sync active attack state */
        if (st->wifi_scanner) {
            st->active_attack = wifi_scanner_get_active_attack(st->wifi_scanner);
        }
    }

    /* Clean‑up */
    furi_timer_stop(st->anim_timer);
    furi_timer_free(st->anim_timer);
    gui_remove_view_port(st->gui, st->vp);
    view_port_free(st->vp);
    furi_record_close(st->gui);
    
    if (st->wifi_scanner) {
        wifi_scanner_free(st->wifi_scanner);
    }
    
    free(st);
}
