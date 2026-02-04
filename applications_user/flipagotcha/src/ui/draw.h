#pragma once
#include <gui/canvas.h>
#include <stdbool.h>

void draw_face(Canvas* canvas,
               int x,
               int y,
               bool eyes_closed,
               bool mouth_frown);
