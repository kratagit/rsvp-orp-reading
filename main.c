#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "font_data.h" // Wkompilowana czcionka

#define MAX_WORDS 10000

// Funkcja obliczająca indeks Optymalnego Punktu Rozpoznawania (ORP)
// Zwraca pozycję litery (od 0), która ma być czerwona.
// Złota reguła ORP (na podstawie badań Spritz):
int GetORPIndex(int wordLength) {
    if (wordLength <= 1) return 0;  // Słowo 1-literowe: 1. litera
    if (wordLength <= 5) return 1;  // Słowo 2-5 liter: 2. litera
    if (wordLength <= 9) return 2;  // Słowo 6-9 liter: 3. litera
    if (wordLength <= 13) return 3; // Słowo 10-13 liter: 4. litera
    return 4;                       // Słowo powyżej 13 liter: 5. litera
}

// Funkcja rysująca słowo RSVP
void DrawRSVPWord(Font font, const char* word, float fontSize, int centerX, int centerY) {
    // Używamy GetCodepoint do poprawnego liczenia znaków Unicode (UTF-8)
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(word, &codepointCount);
    
    if (codepointCount == 0) {
        UnloadCodepoints(codepoints);
        return;
    }

    int orpIdx = GetORPIndex(codepointCount);

    // Dzielenie słowa na 3 części (w znakach Unicode)
    // Ponieważ znaki UTF-8 mogą zajmować więcej niż 1 bajt, musimy zrekonstruować stringi
    char left[256] = {0};
    char center[8] = {0};
    char right[256] = {0};
    
    int byteOffset = 0;
    int currentCodepoint = 0;
    
    while (currentCodepoint < codepointCount) {
        int codepointSize = 0;
        GetCodepoint(&word[byteOffset], &codepointSize);
        
        if (currentCodepoint < orpIdx) {
            strncat(left, &word[byteOffset], codepointSize);
        } else if (currentCodepoint == orpIdx) {
            strncat(center, &word[byteOffset], codepointSize);
        } else {
            strncat(right, &word[byteOffset], codepointSize);
        }
        
        byteOffset += codepointSize;
        currentCodepoint++;
    }
    
    UnloadCodepoints(codepoints);

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
    const int screenWidth = 1500;
    const int screenHeight = 600;
    
    // Włączamy antyaliasing (MSAA 4x) przed inicjalizacją okna
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    InitWindow(screenWidth, screenHeight, "C Speed Reader - RSVP");
    SetTargetFPS(60);

    // 2. Przygotowanie tekstu
    char* sourceText = LoadFileText("tekst.txt");
    if (sourceText == NULL) {
        printf("Błąd: Nie można wczytać pliku tekst.txt\n");
        // Fallback jeśli plik nie istnieje, żeby aplikacja się nie wywalała
        sourceText = strdup("Witaj w aplikacji do szybkiego czytania! "
                            "Ta metoda nazywa sie RSVP. "
                            "Zamiast ruszac oczami po calej stronie, koncentrujesz wzrok w jednym punkcie. "
                            "To drastycznie zmniejsza zmeczenie i pozwala czytac z predkoscia nawet do tysiaca slow na minute. "
                            "Nacisnij spacje aby zapauzowac. "
                            "Uzyj strzalek w gore i w dol, aby zmienic predkosc. "
                            "Uzyj strzalek w lewo i prawo, aby przewijac tekst recznie. "
                            "Powodzenia w treningu!");
    }

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

    // Tablica kodów znaków dla języka polskiego (i podstawowych ASCII)
    int codepoints[512];
    for (int i = 0; i < 256; i++) codepoints[i] = i; // Podstawowe ASCII
    
    // Polskie znaki (Unicode)
    int plCodepoints[] = {
        0x0104, 0x0105, // Ą, ą
        0x0106, 0x0107, // Ć, ć
        0x0118, 0x0119, // Ę, ę
        0x0141, 0x0142, // Ł, ł
        0x0143, 0x0144, // Ń, ń
        0x00D3, 0x00F3, // Ó, ó
        0x015A, 0x015B, // Ś, ś
        0x0179, 0x017A, // Ź, ź
        0x017B, 0x017C, // Ż, ż
        0x2013, 0x2014, 0x201E, 0x201D, 0x201C // Myślniki i cudzysłowy
    };
    
    int codepointCount = 256;
    for (int i = 0; i < 23; i++) {
        codepoints[codepointCount++] = plCodepoints[i];
    }

    // Ładujemy czcionkę z pamięci (wkompilowaną w plik wykonywalny)
    Font font = LoadFontFromMemory(".ttf", Roboto_Regular_ttf, Roboto_Regular_ttf_len, 140, codepoints, codepointCount); 
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR); 
    
    // Ładujemy osobną czcionkę dla małych napisów (HUD) z pamięci
    Font fontHud = LoadFontFromMemory(".ttf", Roboto_Regular_ttf, Roboto_Regular_ttf_len, 30, codepoints, codepointCount);
    SetTextureFilter(fontHud.texture, TEXTURE_FILTER_BILINEAR);

    float fontSize = 140.0f; // Docelowy rozmiar wyświetlania głównego tekstu

    // 4. Główna pętla
    while (!WindowShouldClose()) {
        // --- LOGIKA ---
        
        // Obsługa sterowania
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isPlaying = !isPlaying;
        if (IsKeyPressed(KEY_UP)) wpm += 25;
        if (IsKeyPressed(KEY_DOWN) && wpm > 50) wpm -= 25;
        if (IsKeyPressed(KEY_LEFT) && currentWord > 0) currentWord--;
        if (IsKeyPressed(KEY_RIGHT) && currentWord < wordCount - 1) currentWord++;

        // Obsługa kółka myszy do zmiany prędkości
        float wheelMove = GetMouseWheelMove();
        if (wheelMove > 0) wpm += 10;
        if (wheelMove < 0 && wpm > 50) wpm -= 10;

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
        DrawLine(centerX - 400, centerY - 100, centerX + 400, centerY - 100, DARKGRAY); // Górna belka
        DrawLine(centerX - 400, centerY + 100, centerX + 400, centerY + 100, DARKGRAY); // Dolna belka
        DrawLine(centerX, centerY - 100, centerX, centerY - 70, DARKGRAY);             // Górny wskaźnik ORP
        DrawLine(centerX, centerY + 70, centerX, centerY + 100, DARKGRAY);             // Dolny wskaźnik ORP

        // Rysowanie obecnego słowa
        if (wordCount > 0) {
            DrawRSVPWord(font, words[currentWord], fontSize, centerX, centerY);
        }

        // Rysowanie Interfejsu (HUD)
        float hudFontSize = 30.0f;
        DrawTextEx(fontHud, TextFormat("Prędkość: %d WPM", wpm), (Vector2){30, 30}, hudFontSize, 1, LIGHTGRAY);
        DrawTextEx(fontHud, TextFormat("Słowo: %d / %d", currentWord + 1, wordCount), (Vector2){30, 70}, hudFontSize, 1, LIGHTGRAY);
        
        if (!isPlaying) {
            DrawTextEx(fontHud, "[SPACJA / LPM] Start / Pauza", (Vector2){screenWidth - 450, 30}, hudFontSize, 1, YELLOW);
        } else {
            DrawTextEx(fontHud, "Odtwarzanie...", (Vector2){screenWidth - 250, 30}, hudFontSize, 1, GREEN);
        }
        
        // Pasek postępu na samym dole ekranu
        if (wordCount > 0) {
            float progress = (float)(currentWord + 1) / (float)wordCount;
            DrawRectangle(0, screenHeight - 10, (int)(screenWidth * progress), 10, RED);
        }

        EndDrawing();
    }

    // 5. Sprzątanie
    UnloadFont(font);
    UnloadFont(fontHud);
    free(sourceText); // Zwalniamy pamięć po tekście
    CloseWindow();
    return 0;
}