// https://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
#include "fluid.h"

#define SWAP(x0, x) {f32* tmp = x0; x0 = x; x = tmp;}

i32 FluidIX(i32 x, i32 y) {
    return y * FLUID_SIZE_BUFFERED + x;
}

bool FluidIN(f32 x, f32 y) {
    return x >= 0 && x < FLUID_SIZE_BUFFERED && y >= 0 && y < FLUID_SIZE_BUFFERED;
}

FluidGrid* FluidGridCreate(Arena* arena) {
    FluidGrid* fluid = ArenaPushStruct(arena, FluidGrid);
    fluid->u = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    fluid->v = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    fluid->u_prev = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    fluid->v_prev = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    fluid->dens = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    fluid->dens_prev = ArenaPushArray(arena, f32, FLUID_CELLS_BUFFERED);
    return fluid;
}

void FluidGridClearChanges(FluidGrid* fluid) {
    memset(fluid->dens_prev, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
    memset(fluid->u_prev, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
    memset(fluid->v_prev, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
}

void FluidGridReset(FluidGrid* fluid) {
    memset(fluid->dens, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
    memset(fluid->u, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
    memset(fluid->v, 0, sizeof(f32) * FLUID_CELLS_BUFFERED);
    FluidGridClearChanges(fluid);
}

static void FluidAddSource(f32* x, f32* s) {
    for (i32 i = 0; i < FLUID_CELLS_BUFFERED; i++) {
        x[i] += s[i] * FIXED_DT;
    }
}

static void FluidSetBound(i32 b, f32* x) {
    for (i32 i = 1; i <= FLUID_SIZE; i++) {
        x[FluidIX(0, i)] = (b == 1) ? -x[FluidIX(1, i)] : x[FluidIX(1, i)];
        x[FluidIX(FLUID_SIZE + 1, i)] = (b == 1) ? -x[FluidIX(FLUID_SIZE, i)] : x[FluidIX(FLUID_SIZE, i)];
        x[FluidIX(i, 0)] = (b == 2) ? -x[FluidIX(i, 1)] : x[FluidIX(i, 1)];
        x[FluidIX(i, FLUID_SIZE + 1)] = (b == 2) ? -x[FluidIX(i, FLUID_SIZE)] : x[FluidIX(i, FLUID_SIZE)];
    }

    x[FluidIX(0, 0)] = 0.5f * (x[FluidIX(1, 0)] + x[FluidIX(0, 1)]);
    x[FluidIX(0, FLUID_SIZE + 1)] = 0.5f * (x[FluidIX(1, FLUID_SIZE + 1)] + x[FluidIX(0, FLUID_SIZE)]);
    x[FluidIX(FLUID_SIZE + 1, 0)] = 0.5f * (x[FluidIX(FLUID_SIZE, 0)] + x[FluidIX(FLUID_SIZE + 1, 1)]);
    x[FluidIX(FLUID_SIZE + 1, FLUID_SIZE + 1)] = 0.5f * (x[FluidIX(FLUID_SIZE, FLUID_SIZE + 1)] + x[FluidIX(FLUID_SIZE + 1, FLUID_SIZE)]);
}

static void FluidDiffuse(i32 b, f32* x, f32* x0, f32 diff) {
    f32 a = FIXED_DT * diff * FLUID_CELLS;
    for (i32 k = 0; k < 20; k++) {
        for (i32 i = 1; i <= FLUID_SIZE; i++) {
            for (i32 j = 1; j <= FLUID_SIZE; j++) {
                x[FluidIX(i, j)] = (
                    x0[FluidIX(i, j)] +
                    a * (x[FluidIX(i - 1, j)] +
                    x[FluidIX(i + 1, j)] +
                    x[FluidIX(i, j - 1)] +
                    x[FluidIX(i, j + 1)])
                ) / (1 + 4 * a);
            }
        }
        FluidSetBound(b, x);
    }
}

static void FluidAdvect(i32 b, f32* d, f32* d0, f32* u, f32* v) {
    f32 dt0 = FIXED_DT * FLUID_SIZE;
    for (i32 i = 1; i <= FLUID_SIZE; i++) {
        for (i32 j = 1; j <= FLUID_SIZE; j++) {
            f32 x = i - dt0 * u[FluidIX(i, j)];
            f32 y = j - dt0 * v[FluidIX(i, j)];

            if (x < 0.5f) x = 0.5f;
            if (x > FLUID_SIZE + 0.5f) x = FLUID_SIZE + 0.5f;
            if (y < 0.5f) y = 0.5f;
            if (y > FLUID_SIZE + 0.5f) y = FLUID_SIZE + 0.5f;

            i32 i0 = x;
            i32 i1 = i0 + 1;
            i32 j0 = y;
            i32 j1 = j0 + 1;

            f32 s1 = x - i0;
            f32 s0 = 1 - s1;
            f32 t1 = y - j0;
            f32 t0 = 1 - t1;

            d[FluidIX(i, j)] = s0 * (t0 * d0[FluidIX(i0, j0)] +
                t1 * d0[FluidIX(i0, j1)]) +
                s1 * (t0 * d0[FluidIX(i1, j0)] +
                t1 * d0[FluidIX(i1, j1)]);
        }
    }
    FluidSetBound(b, d);
}

static void FluidProject(f32* u, f32* v, f32* p, f32* div) {
    f32 h = 1.0 / FLUID_SIZE;
    for (i32 i = 1; i <= FLUID_SIZE; i++) {
        for (i32 j = 1; j <= FLUID_SIZE; j++) {
            div[FluidIX(i, j)] = -0.5f * h * (
                u[FluidIX(i + 1, j)] - u[FluidIX(i - 1, j)] +
                v[FluidIX(i, j + 1)] - v[FluidIX(i, j - 1)]
            );
            p[FluidIX(i,j)] = 0;
        }
    }
    FluidSetBound(0, div);
    FluidSetBound(0, p);
    for (i32 k = 0; k < 20; k++) {
        for (i32 i = 1; i <= FLUID_SIZE; i++) {
            for (i32 j = 1; j <= FLUID_SIZE; j++) {
                p[FluidIX(i, j)] = (
                    div[FluidIX(i, j)] +
                    p[FluidIX(i - 1, j)] +
                    p[FluidIX(i + 1, j)] +
                    p[FluidIX(i, j - 1)] +
                    p[FluidIX(i, j + 1)]
                ) / 4;
            }
        }
        FluidSetBound(0, p);
    }
    for (i32 i = 1; i <= FLUID_SIZE; i++) {
        for (i32 j = 1; j <= FLUID_SIZE; j++) {
            u[FluidIX(i,j)] -= 0.5f * (
                p[FluidIX(i + 1, j)] - p[FluidIX(i - 1, j)]
            ) / h;
            v[FluidIX(i,j)] -= 0.5f * (
                p[FluidIX(i, j + 1)]-p[FluidIX(i, j - 1)]
            ) / h;
        }
    }
    FluidSetBound(1, u);
    FluidSetBound(2, v);
}

void FluidDensityStep(f32* x, f32* x0, f32* u, f32* v, f32 diff) {
    FluidAddSource(x, x0);
    SWAP(x0, x);
    FluidDiffuse(0, x, x0, diff);
    SWAP(x0, x);
    FluidAdvect(0, x, x0, u, v);
}

void FluidVelocityStep(f32* u, f32* v, f32* u0, f32* v0, f32 visc) {
    FluidAddSource(u, u0);
    FluidAddSource(v, v0);
    SWAP(u0, u);
    FluidDiffuse(1, u, u0, visc);
    SWAP(v0, v);
    FluidDiffuse(2, v, v0, visc);
    FluidProject(u, v, u0, v0);
    SWAP(u0, u);
    SWAP(v0, v);
    FluidAdvect(1, u, u0, u0, v0);
    FluidAdvect(2, v, v0, u0, v0);
    FluidProject(u, v, u0, v0);
}
