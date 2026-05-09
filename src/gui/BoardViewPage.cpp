#include "gui/BoardViewPage.hpp"
#include "gui/AssetManager.hpp"
#include "gui/BoardRenderer.hpp"
#include <algorithm>

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

#ifdef HAVE_RAYLIB

BoardViewPage::BoardViewPage(AssetManager& am, Font f)
    : assets(am), font(f) {}

void BoardViewPage::beginFrame() {
    BeginDrawing();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    Texture2D bgTex = assets.has("bg") ? assets.get("bg") : Texture2D{0,0,0,0,0};
    if (bgTex.id != 0) {
        DrawTexturePro(bgTex,
            Rectangle{0, 0, (float)bgTex.width, (float)bgTex.height},
            Rectangle{0, 0, (float)sw, (float)sh},
            Vector2{0, 0}, 0.0f, WHITE);
    } else {
        ClearBackground({22, 28, 38, 255});
    }
}

void BoardViewPage::endFrame() {
    EndDrawing();
}

void BoardViewPage::drawTextC(const char* text, int x, int y, int fontSize, Color color) {
    DrawTextEx(font, text, Vector2{(float)x, (float)y}, (float)fontSize, 1.0f, color);
}

void BoardViewPage::drawTextC(const std::string& text, int x, int y, int fontSize, Color color) {
    drawTextC(text.c_str(), x, y, fontSize, color);
}

int BoardViewPage::measureTextC(const char* text, int fontSize) {
    return static_cast<int>(MeasureTextEx(font, text, (float)fontSize, 1.0f).x);
}

bool BoardViewPage::buttonRect(int x, int y, int w, int h, const char* label) {
    Rectangle rect{(float)x, (float)y, (float)w, (float)h};
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);
    bool clicked = isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Texture2D btnTex = assets.has("btnOk") ? assets.get("btnOk") : Texture2D{0,0,0,0,0};
    if (btnTex.id != 0) {
        Color tint = isHover ? (Color){220, 220, 255, 255} : WHITE;
        DrawTexturePro(btnTex,
            Rectangle{0, 0, (float)btnTex.width, (float)btnTex.height},
            rect, Vector2{0, 0}, 0.0f, tint);
    } else {
        DrawRectangleRec(rect, isHover ? (Color){80, 140, 200, 255} : (Color){60, 120, 180, 255});
        DrawRectangleLinesEx(rect, 2, {80, 90, 110, 255});
    }

    int fs = h / 2;
    int tw = measureTextC(label, fs);
    drawTextC(label, x + (w - tw) / 2, y + (h - fs) / 2, fs, WHITE);
    return clicked;
}

bool BoardViewPage::imageButton(int x, int y, int w, int h, const std::string& texName) {
    Rectangle rect{(float)x, (float)y, (float)w, (float)h};
    Vector2 mouse = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mouse, rect);
    bool clicked = isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    Texture2D tex = assets.has(texName) ? assets.get(texName) : Texture2D{0,0,0,0,0};
    if (tex.id != 0) {
        Color tint = isHover ? (Color){220, 220, 255, 255} : WHITE;
        DrawTexturePro(tex,
            Rectangle{0, 0, (float)tex.width, (float)tex.height},
            rect, Vector2{0, 0}, 0.0f, tint);
    } else {
        DrawRectangleRec(rect, isHover ? (Color){80, 140, 200, 255} : (Color){60, 120, 180, 255});
        DrawRectangleLinesEx(rect, 2, WHITE);
    }
    return clicked;
}

BoardViewPageInput BoardViewPage::draw(const GameMap& map, const Position& actorPos,
                                        int collectedMask, bool hasResult, bool isPlaying,
                                        int currentStep, int totalSteps,
                                        BoardRenderer& boardRenderer) {
    BoardViewPageInput input;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    int margin = 20;
    int backBtnH = 40;
    int backBtnW = 120;

    if (buttonRect(margin, margin, backBtnW, backBtnH, "BACK")) {
        input.back = true;
    }

    int boardY = margin + backBtnH + margin;
    int controlsH = 100;
    int boardMaxH = sh - boardY - controlsH - margin;
    if (map.getRows() > 0 && map.getCols() > 0) {
        boardRenderer.render(map, actorPos, collectedMask,
                             margin, boardY,
                             sw - margin * 2, boardMaxH);
    }

    if (hasResult) {
        int btnW = 60;
        int btnH = 40;
        int gap = 15;
        int totalW = btnW * 5 + gap * 4;
        int sx = sw / 2 - totalW / 2;
        int cy = sh - margin - btnH;

        if (imageButton(sx, cy, btnW, btnH, "btnFirst")) input.rewind = true;
        sx += btnW + gap;

        if (imageButton(sx, cy, btnW, btnH, "btnPrev")) input.prev = true;
        sx += btnW + gap;

        std::string playTex = isPlaying ? "btnPause" : "btnPlay";
        if (imageButton(sx, cy, btnW, btnH, playTex)) input.playPause = true;
        sx += btnW + gap;

        if (imageButton(sx, cy, btnW, btnH, "btnNext")) input.next = true;
        sx += btnW + gap;

        if (imageButton(sx, cy, btnW, btnH, "btnLast")) input.end = true;

        int stepSize = std::max(16, sh / 45);
        std::string stepStr = "Step: " + std::to_string(currentStep) + " / " + std::to_string(totalSteps);
        int stw = measureTextC(stepStr.c_str(), stepSize);
        drawTextC(stepStr, sw / 2 - stw / 2, cy - stepSize - 10, stepSize, WHITE);
    }

    return input;
}

#endif
