#include "draw.h"

static inline void set_px(Canvas* canvas, int x, int y) {
    canvas_draw_point(canvas, x, y);
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

static void draw_mouth_neutral(Canvas* canvas, int x, int y) {
    canvas_draw_line(canvas, x + 2, y + 5, x + 5, y + 5);
}

static void draw_mouth_wide_smile(Canvas* canvas, int x, int y) {
    // Wider smile for happy state
    canvas_draw_line(canvas, x + 1, y + 5, x + 2, y + 6);
    canvas_draw_line(canvas, x + 2, y + 6, x + 3, y + 7);
    canvas_draw_line(canvas, x + 3, y + 7, x + 4, y + 7);
    canvas_draw_line(canvas, x + 4, y + 7, x + 5, y + 6);
    canvas_draw_line(canvas, x + 5, y + 6, x + 6, y + 5);
}

static void draw_eyes_focused(Canvas* canvas, int x, int y) {
    // Narrowed eyes for focused/attacking state
    canvas_draw_line(canvas, x + 2, y + 2, x + 3, y + 2);
    canvas_draw_line(canvas, x + 5, y + 2, x + 6, y + 2);
    canvas_draw_line(canvas, x + 2, y + 3, x + 3, y + 3);
    canvas_draw_line(canvas, x + 5, y + 3, x + 6, y + 3);
}

static void draw_eyes_crossed(Canvas* canvas, int x, int y) {
    // Crossed eyes for confused state
    set_px(canvas, x + 2, y + 2);
    set_px(canvas, x + 4, y + 4);
    set_px(canvas, x + 6, y + 2);
    set_px(canvas, x + 4, y + 4);
}

static void draw_eyes_happy(Canvas* canvas, int x, int y) {
    // ^ ^ eyes (curved up)
    canvas_draw_line(canvas, x + 1, y + 3, x + 2, y + 2);
    canvas_draw_line(canvas, x + 2, y + 2, x + 3, y + 3);
    canvas_draw_line(canvas, x + 5, y + 3, x + 6, y + 2);
    canvas_draw_line(canvas, x + 6, y + 2, x + 7, y + 3);
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

void draw_face_expression(Canvas* canvas,
                         int x,
                         int y,
                         FaceExpression expression) {
    /* Clear the area */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y, 14, 18);
    canvas_set_color(canvas, ColorBlack);

    switch (expression) {
        case FACE_HAPPY:
            // ^ ^ with wide smile
            draw_eyes_happy(canvas, x, y);
            draw_mouth_wide_smile(canvas, x, y);
            break;
            
        case FACE_ATTACKING:
            // Focused eyes with determined smile
            draw_eyes_focused(canvas, x, y);
            draw_mouth_smile(canvas, x, y);
            break;
            
        case FACE_SCANNING:
            // Open eyes with neutral mouth
            draw_eyes_open(canvas, x, y);
            draw_mouth_neutral(canvas, x, y);
            break;
            
        case FACE_CONFUSED:
            // Crossed eyes with frown
            draw_eyes_crossed(canvas, x, y);
            draw_mouth_frown(canvas, x, y);
            break;
            
        case FACE_SLEEPING:
            // Closed eyes with neutral mouth
            draw_eyes_closed(canvas, x, y);
            draw_mouth_neutral(canvas, x, y);
            break;
            
        case FACE_NORMAL:
        default:
            // Normal happy face
            draw_eyes_open(canvas, x, y);
            draw_mouth_smile(canvas, x, y);
            break;
    }
}
