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

/* ------------------------------------------------------------------ */
static void draw_callback(Canvas* canvas, void* ctx) {
    UiState* st = ctx;

    /* Clear background (optional) */
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0, 128, 64);
    canvas_set_color(canvas, ColorWhite);

    /* Draw the face */
    draw_face(canvas,
              40,               // x offset
              20,               // y offset
              st->eyes_closed,
              st->mouth_frown);
    
    /* Draw WiFi status */
    canvas_set_font(canvas, FontSecondary);
    if (st->scanning) {
        canvas_draw_str(canvas, 2, 10, "Scanning...");
        
        char net_buf[32];
        snprintf(net_buf, sizeof(net_buf), "Networks: %u", st->network_count);
        canvas_draw_str(canvas, 2, 20, net_buf);
    }
    
    if (st->packet_count > 0) {
        char pkt_buf[32];
        snprintf(pkt_buf, sizeof(pkt_buf), "Packets: %u", st->packet_count);
        canvas_draw_str(canvas, 2, 60, pkt_buf);
    }
}

/* ------------------------------------------------------------------ */
static void input_callback(InputEvent* ev, void* ctx) {
    UiState* st = ctx;
    if (ev->type != InputTypeShort) return;

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
            st->eyes_closed = true;
            // Initiate handshake with first network if available
            if (st->wifi_scanner && st->network_count > 0) {
                wifi_scanner_start_handshake(st->wifi_scanner, 0);
                st->packet_count = wifi_scanner_get_packet_count(st->wifi_scanner);
            }
            break;
        case InputKeyRight:
            st->eyes_closed = false;
            break;
        case InputKeyUp:
            st->mouth_frown = false;   // smile
            break;
        case InputKeyDown:
            st->mouth_frown = true;    // frown
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
        
        /* Update network and packet counts if scanning */
        if (st->scanning && st->wifi_scanner) {
            st->network_count = wifi_scanner_get_network_count(st->wifi_scanner);
            st->packet_count = wifi_scanner_get_packet_count(st->wifi_scanner);
            view_port_update(st->vp);
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
