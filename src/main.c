#include "core.h"
#include "constants.h"
#include "fluid.h"

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
        IntVector2 mouse_fluid_cell_pos = (IntVector2) {
            mouse_pos.x / FLUID_CELL_PIXELS,
            mouse_pos.y / FLUID_CELL_PIXELS,
        };

        if (IsKeyPressed(KEY_SPACE)) { FluidGridReset(fluid); }

        if (FluidIN(mouse_fluid_cell_pos.x, mouse_fluid_cell_pos.y) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_vel = Vector2Subtract(mouse_pos, last_mouse_pos);
            i32 grid_index = FluidIX(mouse_fluid_cell_pos.x, mouse_fluid_cell_pos.y);
            fluid->dens_prev[grid_index] = 20.0f;
            fluid->u_prev[grid_index] += mouse_vel.x;
            fluid->v_prev[grid_index] += mouse_vel.y;
        }

        // Simulate
        while(accumulator >= FIXED_DT) {
            FluidVelocityStep(fluid->u, fluid->v, fluid->u_prev, fluid->v_prev, visc);
            FluidDensityStep(fluid->dens, fluid->dens_prev, fluid->u, fluid->v, diff);
            FluidGridClearChanges(fluid);
            accumulator -= FIXED_DT;
        }

        // Render
        for (i32 y = 0; y < FLUID_SIZE_BUFFERED; y++) {
            for (i32 x = 0; x < FLUID_SIZE_BUFFERED; x++) {
                i32 grid_index = x + y * TEXTURE_WIDTH;
                f32 density = Clamp(fluid->dens[FluidIX(x, y)], 0.0f, 1.0f);
                Color c = (Color) {
                    (u8)(density * density * density * 128),
                    (u8)(density * density * 255),
                    (u8)(density * 255),
                    255
                };
                pixels[grid_index] = c;
            }
        }

        UpdateTexture(texture, pixels);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(texture, (Vector2){0, 0}, 0, FLUID_CELL_PIXELS, WHITE);
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
}
