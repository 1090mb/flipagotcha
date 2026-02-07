#pragma once
#include <gui/canvas.h>
#include <stdbool.h>

// Face expression types
typedef enum {
    FACE_NORMAL,        // Normal happy face
    FACE_HAPPY,         // Extra happy (successful capture)
    FACE_ATTACKING,     // Attacking expression
    FACE_SCANNING,      // Focused/scanning
    FACE_CONFUSED,      // No ESP32 connected
    FACE_SLEEPING       // Idle/not active
} FaceExpression;

void draw_face(Canvas* canvas,
               int x,
               int y,
               bool eyes_closed,
               bool mouth_frown);

void draw_face_expression(Canvas* canvas,
                         int x,
                         int y,
                         FaceExpression expression);
