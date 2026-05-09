#include "raylib.h"

int main() {
    InitWindow(800, 450, "Raylib Test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("raylib works", 260, 200, 30, DARKBLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}