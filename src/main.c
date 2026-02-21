#include "core.h"
#include "constants.h"
#include "fluid.h"

void FluidStampAt(FluidGrid *fluid, f32 radius, i32 cx, i32 cy, Vector2 vel, bool add_fluid) {
    for (i32 y = cy - radius; y < cy + radius; y++) {
        for (i32 x = cx - radius; x < cx + radius; x++) {
            if (!FluidIN(x, y)) continue;

            i32 idx = FluidIX(x, y);
            if (add_fluid) {
                fluid->dens_prev[idx]  = 20.0f;
                fluid->u_prev[idx] += vel.x;
                fluid->v_prev[idx] += vel.y;
            }
            else {
                fluid->solid[idx] = true;
            }
        }
    }
}

void FluidStampLine(FluidGrid *fluid, f32 radius, Vector2 from_screen, Vector2 to_screen, Vector2 vel, bool add_fluid) {
    i32 x0 = (i32)(from_screen.x / FLUID_CELL_PIXELS);
    i32 y0 = (i32)(from_screen.y / FLUID_CELL_PIXELS);
    i32 x1 = (i32)(to_screen.x / FLUID_CELL_PIXELS);
    i32 y1 = (i32)(to_screen.y / FLUID_CELL_PIXELS);

    // Bresenham's line
    i32 dx = abs_i64(x1 - x0);
    i32 sx = x0 < x1 ? 1 : -1;
    i32 dy = -abs_i64(y1 - y0);
    i32 sy = y0 < y1 ? 1 : -1;
    i32 err = dx + dy;

    while (true) {
        FluidStampAt(fluid, radius, x0, y0, vel, add_fluid);

        if (x0 == x1 && y0 == y1) { break; }

        i32 e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_CAPTION);
    SetTargetFPS(WINDOW_FPS);
    SetRandomSeed(0);

    Arena* arena = ArenaCreate(GiB(1), MiB(1));

    FluidGrid* fluid = FluidGridCreate(arena);

    static const u32 TEXTURE_WIDTH = FLUID_CELL_PIXELS * FLUID_SIZE_BUFFERED;
    static const u32 TEXTURE_HEIGHT = FLUID_CELL_PIXELS * FLUID_SIZE_BUFFERED;
    Color* pixels = ArenaPushArray(arena, Color, TEXTURE_WIDTH * TEXTURE_HEIGHT);
    Image* image = ArenaPushStruct(arena, Image);
    image->data = pixels;
    image->width = TEXTURE_WIDTH;
    image->height = TEXTURE_HEIGHT;
    image->mipmaps = 1;
    image->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    Texture2D texture = LoadTextureFromImage(*image);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);

    f32 mouse_radius = 2.0f;
    Vector2 mouse_vel = Vector2Zero();
    Vector2 mouse_pos = GetMousePosition();
    Vector2 last_mouse_pos = mouse_pos;

    f32 visc = 0.0f;
    f32 diff = 0.0f;

    f64 accumulator = 0.0f;

    while (!WindowShouldClose()) {
        f32 dt = GetFrameTime();
        accumulator += dt;

        // User interaction
        last_mouse_pos = mouse_pos;
        mouse_pos = GetMousePosition();
        mouse_vel = Vector2Subtract(mouse_pos, last_mouse_pos);

        if (IsKeyPressed(KEY_SPACE)) { FluidGridReset(fluid); }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            FluidStampLine(fluid, mouse_radius, last_mouse_pos, mouse_pos, mouse_vel, true);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            FluidStampLine(fluid, mouse_radius, last_mouse_pos, mouse_pos, mouse_vel, false);
        }

        // Simulate
        bool fluid_updated = false;
        while(accumulator >= FIXED_DT) {
            FluidVelocityStep(fluid->u, fluid->v, fluid->u_prev, fluid->v_prev, visc, fluid->solid);
            FluidDensityStep(fluid->dens, fluid->dens_prev, fluid->u, fluid->v, diff, fluid->solid);
            FluidGridClearChanges(fluid);
            accumulator -= FIXED_DT;
            fluid_updated = true;
        }

        if (fluid_updated) {
            // Update render texture once for final state
            for (i32 y = 0; y < FLUID_SIZE_BUFFERED; y++) {
                for (i32 x = 0; x < FLUID_SIZE_BUFFERED; x++) {
                    i32 grid_index = x + y * TEXTURE_WIDTH;
                    f32 density = Clamp(fluid->dens[FluidIX(x, y)], 0.0f, 1.0f);
                    Color c = WHITE;
                    if (!fluid->solid[(FluidIX(x, y))]) {
                        c = (Color) {
                            (u8)(density * density * density * 128),
                            (u8)(density * density * 255),
                            (u8)(density * 255),
                            255
                        };
                    }
                    pixels[grid_index] = c;
                }
            }
            UpdateTexture(texture, pixels);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(texture, (Vector2){0, 0}, 0, FLUID_CELL_PIXELS, WHITE);
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
}
