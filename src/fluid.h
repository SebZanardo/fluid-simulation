#ifndef FLUID_H
#define FLUID_H

#include "core.h"
#include "constants.h"

typedef struct {
    f32* u;
    f32* v;
    f32* u_prev;
    f32* v_prev;
    f32* dens;
    f32* dens_prev;
    bool* solid;
} FluidGrid;

static const u32 FLUID_CELL_PIXELS = 8;
static const u32 FLUID_SIZE = 64;
static const u32 FLUID_SIZE_BUFFERED = FLUID_SIZE + 2;
static const u32 FLUID_CELLS = FLUID_SIZE * FLUID_SIZE;
static const u32 FLUID_CELLS_BUFFERED = FLUID_SIZE_BUFFERED * FLUID_SIZE_BUFFERED;

i32 FluidIX(i32 x, i32 y);
bool FluidIN(f32 x, f32 y);
FluidGrid* FluidGridCreate(Arena* arena);
void FluidGridClearChanges(FluidGrid* fluid);
void FluidGridReset(FluidGrid* fluid);
void FluidDensityStep(f32* x, f32* x0, f32* u, f32* v, f32 diff, bool* solid);
void FluidVelocityStep(f32* u, f32* v, f32* u0, f32* v0, f32 visc, bool* solid);

#endif // FLUID_H
