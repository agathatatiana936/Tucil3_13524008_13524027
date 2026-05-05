#include "gui/GuiRenderer.hpp"
#include "gui/AssetManager.hpp"
#include "model/GameMap.hpp"
#include "model/Position.hpp"
#include "model/SearchResult.hpp"
#include <algorithm>

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

#ifdef HAVE_RAYLIB

GuiRenderer::GuiRenderer(AssetManager& am, Font f)
    : assets(am), font(f) {}

void GuiRenderer::beginFrame() {
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

void GuiRenderer::endFrame() {
    EndDrawing();
}

void GuiRenderer::drawTextC(const char* text, int x, int y, int fontSize, Color color) {
    DrawTextEx(font, text, Vector2{(float)x, (float)y}, (float)fontSize, 1.0f, color);
}

void GuiRenderer::drawTextC(const std::string& text, int x, int y, int fontSize, Color color) {
    drawTextC(text.c_str(), x, y, fontSize, color);
}

int GuiRenderer::measureTextC(const char* text, int fontSize) {
    return static_cast<int>(MeasureTextEx(font, text, (float)fontSize, 1.0f).x);
}

int GuiRenderer::measureTextC(const std::string& text, int fontSize) {
    return measureTextC(text.c_str(), fontSize);
}

void GuiRenderer::drawPanel(int x, int y, int w, int h, Color fill, Color border) {
    DrawRectangle(x, y, w, h, fill);
    DrawRectangleLinesEx(Rectangle{(float)x, (float)y, (float)w, (float)h}, 2, border);
}

bool GuiRenderer::radioButton(int x, int y, const char* label, bool selected, int fontSize) {
    int size = 22;
    std::string texName = selected ? "radioOn" : "radioOff";
    Texture2D tex = assets.has(texName) ? assets.get(texName) : Texture2D{0,0,0,0,0};

    if (tex.id != 0) {
        DrawTexturePro(tex,
            Rectangle{0, 0, (float)tex.width, (float)tex.height},
            Rectangle{(float)x, (float)y, (float)size, (float)size},
            Vector2{0, 0}, 0.0f, WHITE);
    } else {
        DrawCircleLines(x + size / 2, y + size / 2, size / 2.0f, WHITE);
        if (selected) {
            DrawCircle(x + size / 2, y + size / 2, size / 2.0f - 4, {100, 200, 100, 255});
        }
    }

    drawTextC(label, x + size + 8, y + (size - fontSize) / 2, fontSize, WHITE);

    int lw = measureTextC(label, fontSize);
    Rectangle hit{(float)x, (float)y, (float)(size + 10 + lw + 20), (float)size};
    return CheckCollisionPointRec(GetMousePosition(), hit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool GuiRenderer::buttonRect(int x, int y, int w, int h, const char* label) {
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

bool GuiRenderer::imageButton(int x, int y, int w, int h, const std::string& texName) {
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

void GuiRenderer::drawStat(int x, int y, const char* key, const char* val,
                           Color valColor, int fontSize) {
    drawTextC(key, x, y, fontSize, LIGHTGRAY);
    int kw = measureTextC(key, fontSize);
    drawTextC(val, x + kw + 8, y, fontSize, valColor);
}

LeftPanelInput GuiRenderer::drawLeftPanel(const std::string& selectedAlgo,
                                           const std::string& selectedHeuristic) {
    LeftPanelInput input;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = std::max(200, sw * 18 / 100);
    int margin = 10;
    int ph = sh - margin * 2;

    drawPanel(margin, margin, pw, ph, {32, 42, 56, 240}, {55, 65, 85, 255});

    int x = margin + 20;
    int y = margin + 20;
    int headingSize = std::max(18, sh / 40);
    int itemSize = std::max(14, sh / 50);

    drawTextC("ALGORITHM", x, y, headingSize, {180, 200, 230, 255});
    y += headingSize + 15;

    if (radioButton(x, y, "A*", selectedAlgo == "A*", itemSize)) input.algoAStar = true;
    y += 30;
    if (radioButton(x, y, "UCS", selectedAlgo == "UCS", itemSize)) input.algoUCS = true;
    y += 30;
    if (radioButton(x, y, "GBFS", selectedAlgo == "GBFS", itemSize)) input.algoGBFS = true;
    y += headingSize + 25;

    drawTextC("HEURISTIC", x, y, headingSize, {180, 200, 230, 255});
    y += headingSize + 15;

    if (radioButton(x, y, "H1 - Basic", selectedHeuristic == "H1", itemSize)) input.heuristicH1 = true;
    y += 30;
    if (radioButton(x, y, "H2 - Custom", selectedHeuristic == "H2", itemSize)) input.heuristicH2 = true;
    y += 30;
    if (radioButton(x, y, "H3 - Advanced", selectedHeuristic == "H3", itemSize)) input.heuristicH3 = true;
    y += headingSize + 25;

    int btnH = std::max(36, sh / 20);
    if (buttonRect(x, y, pw - 40, btnH, "SOLVE")) input.solveClicked = true;

    return input;
}

RightPanelInput GuiRenderer::drawRightPanel(const std::string& filename,
                                             const std::string& statusMsg,
                                             bool statusOk,
                                             bool hasResult,
                                             const SearchResult& result) {
    RightPanelInput input;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int pw = std::max(240, sw * 22 / 100);
    int margin = 10;
    int px = sw - pw - margin;
    int ph = sh - margin * 2;

    drawPanel(px, margin, pw, ph, {32, 42, 56, 240}, {55, 65, 85, 255});

    int x = px + 20;
    int y = margin + 20;
    int headingSize = std::max(18, sh / 40);
    int itemSize = std::max(14, sh / 50);
    int smallSize = std::max(12, sh / 60);

    drawTextC("FILE", x, y, headingSize, {180, 200, 230, 255});
    y += headingSize + 15;

    if (!filename.empty()) {
        drawTextC(filename, x, y, itemSize, {150, 180, 220, 255});
        y += itemSize + 8;
    }

    int btnH = std::max(32, sh / 22);
    if (buttonRect(x, y, pw - 40, btnH, "LOAD FILE")) input.loadClicked = true;
    y += btnH + 12;

    if (!statusMsg.empty()) {
        Color c = statusOk ? (Color){100, 230, 100, 255} : (Color){230, 100, 100, 255};
        drawTextC(statusMsg, x, y, smallSize, c);
        y += smallSize + 8;
    }

    y += 10;
    DrawLine(x, y, x + pw - 40, y, {55, 65, 85, 255});
    y += 15;

    drawTextC("STATISTICS", x, y, headingSize, {180, 200, 230, 255});
    y += headingSize + 15;

    if (hasResult) {
        const char* found = result.isSolutionFound() ? "YES" : "NO";
        drawStat(x, y, "Found:", found, result.isSolutionFound() ? GREEN : RED, itemSize);
        y += itemSize + 8;

        drawTextC("Path:", x, y, itemSize, LIGHTGRAY);
        y += itemSize + 4;
        {
            const std::string& path = result.getSolutionPath();
            int maxLineW = pw - 40;
            int lineX = x;
            int lineY = y;
            size_t start = 0;
            while (start < path.length()) {
                size_t len = 1;
                while (start + len <= path.length() &&
                       measureTextC(path.substr(start, len), itemSize) < maxLineW) {
                    ++len;
                }
                if (len > 1 && start + len <= path.length()) --len;
                drawTextC(path.substr(start, len), lineX, lineY, itemSize, WHITE);
                lineY += itemSize + 2;
                start += len;
            }
            y = lineY + 4;
        }

        drawStat(x, y, "Cost:", std::to_string(result.getTotalCost()).c_str(), WHITE, itemSize);
        y += itemSize + 8;

        std::string timeStr = std::to_string(static_cast<float>(result.getExecutionTimeMs())) + " ms";
        drawStat(x, y, "Time:", timeStr.c_str(), WHITE, itemSize);
        y += itemSize + 8;

        drawStat(x, y, "Iterations:", std::to_string(result.getIterationsCount()).c_str(), WHITE, itemSize);
        y += itemSize + 8;

        if (!result.getMessage().empty()) {
            drawTextC(result.getMessage(), x, y, smallSize, GRAY);
            y += smallSize + 8;
        }

        y += 15;
        if (buttonRect(x, y, pw - 40, btnH, "SAVE AS...")) input.saveClicked = true;
    } else {
        drawTextC("No result yet.", x, y, itemSize, GRAY);
    }

    return input;
}

CenterPanelInput GuiRenderer::drawCenterPanel(const GameMap& map,
                                               const Position& actorPos,
                                               int collectedMask,
                                               bool hasResult,
                                               bool isPlaying,
                                               int currentStep,
                                               int totalSteps) {
    (void)actorPos;
    (void)collectedMask;
    CenterPanelInput input;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int leftW = std::max(200, sw * 18 / 100);
    int rightW = std::max(240, sw * 22 / 100);
    int margin = 10;
    int x = leftW + margin * 2;
    int w = sw - leftW - rightW - margin * 4;
    int h = sh;

    bool isLargeBoard = map.getRows() > 10 || map.getCols() > 10;

    if (isLargeBoard && map.getRows() > 0 && map.getCols() > 0) {
        int btnW = std::min(250, w - 40);
        int btnH = std::max(45, sh / 16);
        int bx = x + (w - btnW) / 2;
        int by = h / 2 - btnH / 2;

        drawTextC("Ukuran Boardnya kegedean, cek aja dihalaman ini", x + (w - measureTextC("Ukuran Boardnya kegedean, cek aja dihalaman ini", 18)) / 2,
                  by - 40, 18, BLACK);
        if (buttonRect(bx, by, btnW, btnH, "VIEW BOARD")) {
            input.viewBoard = true;
        }
        return input;
    }

    Texture2D boardTex = assets.has("board") ? assets.get("board") : Texture2D{0,0,0,0,0};
    if (boardTex.id != 0) {
        DrawTexturePro(boardTex,
            Rectangle{0, 0, (float)boardTex.width, (float)boardTex.height},
            Rectangle{(float)x, (float)margin, (float)w, (float)(h - margin * 2 - 100)},
            Vector2{0, 0}, 0.0f, WHITE);
    }

    if (map.getRows() == 0 || map.getCols() == 0) {
        int msgSize = std::max(18, sh / 40);
        const char* msg = "Load a file to display the board";
        int tw = measureTextC(msg, msgSize);
        drawTextC(msg, x + (w - tw) / 2, h / 2 - msgSize / 2, msgSize, GRAY);
    }

    if (hasResult) {
        int btnW = 55;
        int btnH = 35;
        int gap = 12;
        int totalW = btnW * 5 + gap * 4;
        int sx = x + w / 2 - totalW / 2;
        int cy = h - margin - 70;

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

        int stepSize = std::max(14, sh / 50);
        std::string stepStr = "Step: " + std::to_string(currentStep) + " / " + std::to_string(totalSteps);
        int stw = measureTextC(stepStr, stepSize);
        drawTextC(stepStr, x + w / 2 - stw / 2, cy - stepSize - 8, stepSize, WHITE);
    }

    return input;
}

BoardViewInput GuiRenderer::drawBoardViewPage(const GameMap& map,
                                               bool hasResult,
                                               bool isPlaying,
                                               int currentStep,
                                               int totalSteps) {
    (void)map;
    BoardViewInput input;
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

    int margin = 20;
    int backBtnH = 40;
    int backBtnW = 120;

    if (buttonRect(margin, margin, backBtnW, backBtnH, "BACK")) {
        input.back = true;
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
        int stw = measureTextC(stepStr, stepSize);
        drawTextC(stepStr, sw / 2 - stw / 2, cy - stepSize - 10, stepSize, WHITE);
    }

    return input;
}

#endif
