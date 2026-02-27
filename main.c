#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "font_data.h"

#define MAX_WORDS 10000

// --- External Assembly Functions ---
// String length calculation (replaces strlen)
extern int FastStrLen(const char* str);

// ORP index calculation in assembly
extern int GetORPIndex_Asm(int wordLength);
// -----------------------------------

// Calculates the Optimal Recognition Point (ORP) index
// Returns the index of the letter that should be colored red.
int GetORPIndex(int wordLength) {
    return GetORPIndex_Asm(wordLength);
}

// Draws the RSVP word
void DrawRSVPWord(Font font, const char* word, float fontSize, int centerX, int centerY) {
    // Use GetCodepoint for correct Unicode (UTF-8) counting
    int codepointCount = 0;
    int* codepoints = LoadCodepoints(word, &codepointCount);
    
    if (codepointCount == 0) {
        UnloadCodepoints(codepoints);
        return;
    }

    int orpIdx = GetORPIndex(codepointCount);

    // Split word into 3 parts (unicode aware)
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

    float spacing = 2.0f;

    // Measure width of each part
    Vector2 leftSize = MeasureTextEx(font, left, fontSize, spacing);
    Vector2 centerSize = MeasureTextEx(font, center, fontSize, spacing);
    
    // Calculate precise X positions to center the ORP character
    float centerCharX = centerX - (centerSize.x / 2.0f);
    float leftPartX = centerCharX - leftSize.x - (FastStrLen(left) > 0 ? spacing : 0);
    float rightPartX = centerCharX + centerSize.x + spacing;
    
    float textY = centerY - (centerSize.y / 2.0f);

    DrawTextEx(font, left, (Vector2){leftPartX, textY}, fontSize, spacing, WHITE);
    DrawTextEx(font, center, (Vector2){centerCharX, textY}, fontSize, spacing, RED);
    DrawTextEx(font, right, (Vector2){rightPartX, textY}, fontSize, spacing, WHITE);
}

int main(void) {
    // 1. Initialization
    const int screenWidth = 1500;
    const int screenHeight = 600;
    
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    InitWindow(screenWidth, screenHeight, "C Speed Reader - RSVP");
    SetTargetFPS(60);

    // 2. Prepare text
    char* sourceText = LoadFileText("tekst.txt");
    if (sourceText == NULL) {
        printf("Error: Could not load tekst.txt\n");
        sourceText = strdup("Welcome to the speed reading app! "
                            "This method is called RSVP. "
                            "Instead of moving your eyes, focus on one point. "
                            "This reduces fatigue and increases reading speed. "
                            "Press SPACE to pause. "
                            "Use UP/DOWN arrows to change speed. "
                            "Use LEFT/RIGHT arrows for manual navigation. "
                            "Good luck!");
    }

    // Split text into words array
    char* words[MAX_WORDS];
    int wordCount = 0;
    
    char* token = strtok(sourceText, " \n\t");
    while (token != NULL && wordCount < MAX_WORDS) {
        words[wordCount++] = token;
        token = strtok(NULL, " \n\t");
    }

    // 3. Application State
    int currentWord = 0;
    int wpm = 250;
    bool isPlaying = false;
    float timer = 0.0f;

    // Codepoints for Polish language and ASCII
    int codepoints[512];
    for (int i = 0; i < 256; i++) codepoints[i] = i; 
    
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
        0x2013, 0x2014, 0x201E, 0x201D, 0x201C
    };
    
    int codepointCount = 256;
    for (int i = 0; i < 23; i++) {
        codepoints[codepointCount++] = plCodepoints[i];
    }

    // Load font from memory
    Font font = LoadFontFromMemory(".ttf", Roboto_Regular_ttf, Roboto_Regular_ttf_len, 140, codepoints, codepointCount); 
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR); 
    
    Font fontHud = LoadFontFromMemory(".ttf", Roboto_Regular_ttf, Roboto_Regular_ttf_len, 30, codepoints, codepointCount);
    SetTextureFilter(fontHud.texture, TEXTURE_FILTER_BILINEAR);

    float fontSize = 140.0f;

    // 4. Main loop
    while (!WindowShouldClose()) {
        // --- LOGIC ---
        
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isPlaying = !isPlaying;
        if (IsKeyPressed(KEY_UP)) wpm += 25;
        if (IsKeyPressed(KEY_DOWN) && wpm > 50) wpm -= 25;
        if (IsKeyPressed(KEY_LEFT) && currentWord > 0) currentWord--;
        if (IsKeyPressed(KEY_RIGHT) && currentWord < wordCount - 1) currentWord++;

        float wheelMove = GetMouseWheelMove();
        if (wheelMove > 0) wpm += 10;
        if (wheelMove < 0 && wpm > 50) wpm -= 10;

        if (isPlaying && wordCount > 0) {
            timer += GetFrameTime();
            float timePerWord = 60.0f / (float)wpm;

            if (timer >= timePerWord) {
                timer = 0.0f;
                currentWord++;
                
                if (currentWord >= wordCount) {
                    currentWord = wordCount - 1;
                    isPlaying = false;
                }
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground((Color){ 15, 15, 15, 255 });

        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        // Guides
        DrawLine(centerX - 400, centerY - 100, centerX + 400, centerY - 100, DARKGRAY);
        DrawLine(centerX - 400, centerY + 100, centerX + 400, centerY + 100, DARKGRAY);
        DrawLine(centerX, centerY - 100, centerX, centerY - 70, DARKGRAY);
        DrawLine(centerX, centerY + 70, centerX, centerY + 100, DARKGRAY);

        if (wordCount > 0) {
            DrawRSVPWord(font, words[currentWord], fontSize, centerX, centerY);
        }

        // HUD
        float hudFontSize = 30.0f;
        DrawTextEx(fontHud, TextFormat("Speed: %d WPM", wpm), (Vector2){30, 30}, hudFontSize, 1, LIGHTGRAY);
        DrawTextEx(fontHud, TextFormat("Word: %d / %d", currentWord + 1, wordCount), (Vector2){30, 70}, hudFontSize, 1, LIGHTGRAY);
        
        if (!isPlaying) {
            DrawTextEx(fontHud, "[SPACE / LMB] Start / Pause", (Vector2){screenWidth - 450, 30}, hudFontSize, 1, YELLOW);
        } else {
            DrawTextEx(fontHud, "Playing...", (Vector2){screenWidth - 250, 30}, hudFontSize, 1, GREEN);
        }
        
        // Progress bar
        if (wordCount > 0) {
            float progress = (float)(currentWord + 1) / (float)wordCount;
            DrawRectangle(0, screenHeight - 10, (int)(screenWidth * progress), 10, RED);
        }

        EndDrawing();
    }

    // 5. Cleanup
    UnloadFont(font);
    UnloadFont(fontHud);
    free(sourceText);
    CloseWindow();
    return 0;
}