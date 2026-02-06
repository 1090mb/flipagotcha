#include "draw.h"

static inline void set_px(Canvas* canvas, int x, int y) {
    canvas_draw_dot(canvas, x, y);
}

/* ------------------------------------------------------------------ */
static void draw_eyes_open(Canvas* canvas, int x, int y) {
    set_px(canvas, x + 2, y + 2);
    set_px(canvas, x + 5, y + 2);
}

static void draw_eyes_closed(Canvas* canvas, int x, int y) {
    canvas_draw_line(canvas, x + 2, y + 4, x + 3, y + 4);
    canvas_draw_line(canvas, x + 5, y + 4, x + 6, y + 4);
}

/* ------------------------------------------------------------------ */
static void draw_mouth_smile(Canvas* canvas, int x, int y) {
    canvas_draw_line(canvas, x + 2, y + 5, x + 3, y + 6);
    canvas_draw_line(canvas, x + 3, y + 6, x + 4, y + 6);
    canvas_draw_line(canvas, x + 4, y + 6, x + 5, y + 5);
}

static void draw_mouth_frown(Canvas* canvas, int x, int y) {
    canvas_draw_line(canvas, x + 2, y + 5, x + 3, y + 4);
    canvas_draw_line(canvas, x + 3, y + 4, x + 4, y + 4);
    canvas_draw_line(canvas, x + 4, y + 4, x + 5, y + 5);
}

/* ------------------------------------------------------------------ */
void draw_face(Canvas* canvas,
               int x,
               int y,
               bool eyes_closed,
               bool mouth_frown) {

    /* Optional clear of the area (helps when switching expressions) */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y, 12, 16);
    canvas_set_color(canvas, ColorBlack);

    if (eyes_closed) {
        draw_eyes_closed(canvas, x, y);
    } else {
        draw_eyes_open(canvas, x, y);
    }

    if (mouth_frown) {
        draw_mouth_frown(canvas, x, y);
    } else {
        draw_mouth_smile(canvas, x, y);
    }
}
