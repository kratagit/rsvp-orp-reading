#include "raylib.h"

int main(void) {
    // Inicjalizacja okna 800x400
    InitWindow(800, 400, "C Speed Reader - Test");
    SetTargetFPS(60);

    // Główna pętla programu
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); // Czarne tło

        // Rysowanie testowych linii (prowadnic)
        DrawLine(400, 100, 400, 150, DARKGRAY); 
        DrawLine(400, 250, 400, 300, DARKGRAY); 
        DrawLine(100, 100, 700, 100, DARKGRAY); 
        DrawLine(100, 300, 700, 300, DARKGRAY); 

        // Rysowanie testowego słowa na środku
        DrawText("Hello World!", 300, 180, 30, WHITE);

        EndDrawing();
    }

    // Zamknięcie okna
    CloseWindow();
    return 0;
}