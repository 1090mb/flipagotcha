#include "view.h"
#include "draw.h"
#include "../hw/uart.h"
#include "../common/protocol.h"
#include <furi.h>
#include <input/input.h>

#define ANIM_TICK_MS 100

/* ------------------------------------------------------------------ */
static void draw_callback(Canvas* canvas, void* ctx) {
    UiState* st = ctx;

    /* Clear background (optional) */
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0, 128, 64, ColorBlack);
    canvas_set_color(canvas, ColorWhite);

    /* Draw the face */
    draw_face(canvas,
              40,               // x offset
              20,               // y offset
              st->eyes_closed,
              st->mouth_frown);
}

/* ------------------------------------------------------------------ */
static void input_callback(InputEvent* ev, void* ctx) {
    UiState* st = ctx;
    if (ev->type != InputTypeShort) return;

    switch (ev->key) {
        case InputKeyOk:
            {
                uint8_t cmd = CMD_TOGGLE_SCAN;
                uart_write(&cmd, 1);
                st->scanning = !st->scanning;
            }
            break;
        case InputKeyLeft:
            st->eyes_closed = true;
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
                    if (blink_ctr >= 4) {          # toggle every 4 ticks ≈400 ms
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

        /* ViewPort creation */
        st->vp = view_port_alloc();
        view_port_draw_callback_set(st->vp, draw_callback, st);
        view_port_input_callback_set(st->vp, input_callback, st);

        /* Attach to global GUI */
        st->gui = furi_record_open("gui");
        gui_add_view_port(st->gui, st->vp, GuiLayerFullscreen);

        /* Animation timer */
        st->anim_timer = furi_timer_alloc(anim_tick, FuriTimerTypePeriodic, st);
        furi_timer_start(st->anim_timer, ANIM_TICK_MS);

        /* Keep thread alive until the user exits */
        while (!furi_thread_is_stopped(furi_thread_get_current())) {
            furi_delay_ms(200);
        }

        /* Clean‑up */
        furi_timer_stop(st->anim_timer);
        furi_timer_free(st->anim_timer);
        gui_remove_view_port(st->gui, st->vp);
        view_port_free(st->vp);
        furi_record_close(st->gui);
        free(st);
    }
