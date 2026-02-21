#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "core.h"

static const u32 WINDOW_WIDTH = 520;
static const u32 WINDOW_HEIGHT = 520;
static const Vector2 WINDOW_SIZE = {WINDOW_WIDTH, WINDOW_HEIGHT};
static const char WINDOW_CAPTION[] = "fluid-simulation";
static const u32 WINDOW_FPS = 60;
static const f32 FIXED_DT = 1.0f / 30.0f;

#endif // CONSTANTS_H
