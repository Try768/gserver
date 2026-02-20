//#define CloseWindow CloseWindowRaylib
#define RAYLIB_STATIC
#define USE_LIBTYPE_SHARED
#include "raylib/raylib.h"

int main()
{
    InitWindow(800, 600, "Clear Only");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(10,10,100,100,RED);
        EndDrawing();
    }
    CloseWindow();
}
