#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_WORDS 5000

// Funkcja obliczająca indeks Optymalnego Punktu Rozpoznawania (ORP)
// Zwraca pozycję litery (od 0), która ma być czerwona.
int GetORPIndex(int wordLength) {
    if (wordLength <= 1) return 0;
    if (wordLength <= 3) return 1;
    if (wordLength <= 5) return 2;
    if (wordLength <= 9) return 3;
    if (wordLength <= 13) return 4;
    return 5;
}

// Funkcja rysująca słowo RSVP
void DrawRSVPWord(Font font, const char* word, float fontSize, int centerX, int centerY) {
    int len = strlen(word);
    if (len == 0) return;

    int orpIdx = GetORPIndex(len);

    // Dzielenie słowa na 3 części
    char left[64] = {0};
    char center[2] = {0};
    char right[64] = {0};

    strncpy(left, word, orpIdx);       // Lewa część (przed czerwoną literą)
    center[0] = word[orpIdx];          // Czerwona litera
    if (len > orpIdx + 1) {
        strcpy(right, word + orpIdx + 1); // Prawa część (za czerwoną literą)
    }

    float spacing = 2.0f; // Odstęp między literami

    // Mierzenie szerokości każdej z części
    Vector2 leftSize = MeasureTextEx(font, left, fontSize, spacing);
    Vector2 centerSize = MeasureTextEx(font, center, fontSize, spacing);
    
    // Obliczanie precyzyjnych pozycji X, tak by środek 'center' był dokładnie w 'centerX'
    float centerCharX = centerX - (centerSize.x / 2.0f);
    float leftPartX = centerCharX - leftSize.x - (strlen(left) > 0 ? spacing : 0);
    float rightPartX = centerCharX + centerSize.x + spacing;
    
    // Pozycja Y wyśrodkowana
    float textY = centerY - (centerSize.y / 2.0f);

    // Rysowanie
    DrawTextEx(font, left, (Vector2){leftPartX, textY}, fontSize, spacing, WHITE);
    DrawTextEx(font, center, (Vector2){centerCharX, textY}, fontSize, spacing, RED);
    DrawTextEx(font, right, (Vector2){rightPartX, textY}, fontSize, spacing, WHITE);
}

int main(void) {
    // 1. Inicjalizacja
    const int screenWidth = 800;
    const int screenHeight = 400;
    InitWindow(screenWidth, screenHeight, "C Speed Reader - RSVP");
    SetTargetFPS(60);

    // 2. Przygotowanie tekstu (w przyszłości tutaj wczytasz plik za pomocą np. LoadFileText)
    char sourceText[] = "Witaj w aplikacji do szybkiego czytania! "
                        "Ta metoda nazywa sie RSVP. "
                        "Zamiast ruszac oczami po calej stronie, koncentrujesz wzrok w jednym punkcie. "
                        "To drastycznie zmniejsza zmeczenie i pozwala czytac z predkoscia nawet do tysiaca slow na minute. "
                        "Nacisnij spacje aby zapauzowac. "
                        "Uzyj strzalek w gore i w dol, aby zmienic predkosc. "
                        "Uzyj strzalek w lewo i prawo, aby przewijac tekst recznie. "
                        "Powodzenia w treningu!";

    // Rozbijanie tekstu na tablicę słów
    char* words[MAX_WORDS];
    int wordCount = 0;
    
    char* token = strtok(sourceText, " \n\t");
    while (token != NULL && wordCount < MAX_WORDS) {
        words[wordCount++] = token;
        token = strtok(NULL, " \n\t");
    }

    // 3. Zmienne stanu aplikacji
    int currentWord = 0;
    int wpm = 250;                     // Początkowa prędkość
    bool isPlaying = false;            // Czy tekst leci (pauza na start)
    float timer = 0.0f;

    Font font = GetFontDefault();      // Pobieramy domyślną czcionkę systemową
    float fontSize = 40.0f;

    // 4. Główna pętla
    while (!WindowShouldClose()) {
        // --- LOGIKA ---
        
        // Obsługa sterowania
        if (IsKeyPressed(KEY_SPACE)) isPlaying = !isPlaying;
        if (IsKeyPressed(KEY_UP)) wpm += 25;
        if (IsKeyPressed(KEY_DOWN) && wpm > 50) wpm -= 25;
        if (IsKeyPressed(KEY_LEFT) && currentWord > 0) currentWord--;
        if (IsKeyPressed(KEY_RIGHT) && currentWord < wordCount - 1) currentWord++;

        // Obliczanie czasu zmiany słowa
        if (isPlaying && wordCount > 0) {
            timer += GetFrameTime(); // Dodaje czas od ostatniej klatki (ok 0.016s dla 60FPS)
            float timePerWord = 60.0f / (float)wpm;

            if (timer >= timePerWord) {
                timer = 0.0f;
                currentWord++;
                
                // Zatrzymanie na końcu tekstu
                if (currentWord >= wordCount) {
                    currentWord = wordCount - 1;
                    isPlaying = false;
                }
            }
        }

        // --- RYSOWANIE ---
        BeginDrawing();
        ClearBackground((Color){ 15, 15, 15, 255 }); // Ciemnoszare tło (lepsze dla oczu niż czyste czarne)

        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        // Prowadnice (linie na wzór tych z Twojego screena)
        DrawLine(centerX - 150, centerY - 40, centerX + 150, centerY - 40, DARKGRAY); // Górna belka
        DrawLine(centerX - 150, centerY + 40, centerX + 150, centerY + 40, DARKGRAY); // Dolna belka
        DrawLine(centerX, centerY - 40, centerX, centerY - 25, DARKGRAY);             // Górny wskaźnik ORP
        DrawLine(centerX, centerY + 25, centerX, centerY + 40, DARKGRAY);             // Dolny wskaźnik ORP

        // Rysowanie obecnego słowa
        if (wordCount > 0) {
            DrawRSVPWord(font, words[currentWord], fontSize, centerX, centerY);
        }

        // Rysowanie Interfejsu (HUD)
        DrawText(TextFormat("Prędkość: %d WPM", wpm), 20, 20, 20, LIGHTGRAY);
        DrawText(TextFormat("Słowo: %d / %d", currentWord + 1, wordCount), 20, 50, 20, LIGHTGRAY);
        
        if (!isPlaying) {
            DrawText("[SPACJA] Start / Pauza", screenWidth - 250, 20, 20, YELLOW);
        } else {
            DrawText("Odtwarzanie...", screenWidth - 180, 20, 20, GREEN);
        }
        
        // Pasek postępu na samym dole ekranu
        if (wordCount > 0) {
            float progress = (float)(currentWord + 1) / (float)wordCount;
            DrawRectangle(0, screenHeight - 5, (int)(screenWidth * progress), 5, RED);
        }

        EndDrawing();
    }

    // 5. Sprzątanie
    CloseWindow();
    return 0;
}