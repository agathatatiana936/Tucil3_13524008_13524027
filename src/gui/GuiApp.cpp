#include "gui/GuiApp.hpp"
#include "gui/portable-file-dialogs.h"
#include "utils/parser.hpp"
#include "utils/InputValidator.hpp"
#include "utils/SolutionSaver.hpp"
#include "utils/MovementEngine.hpp"
#include "solver/AStar.hpp"
#include "heuristic/Custom.hpp"
#include "heuristic/Advanced.hpp"
#include "exception/GameException.hpp"
#include <algorithm>
#include <cctype>
#include <memory>

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

#ifdef HAVE_RAYLIB

namespace {

std::string toUpperStr(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return r;
}

std::string pickFileOpen() {
    auto f = pfd::open_file("Select Input File", ".",
                            {"Text Files", "*.txt"},
                            pfd::opt::none);
    std::vector<std::string> res = f.result();
    if (!res.empty()) return res[0];
    return "";
}

std::string pickFileSave() {
    auto f = pfd::save_file("Save Solution", "solution.txt",
                            {"Text Files", "*.txt"},
                            pfd::opt::force_overwrite);
    std::string path = f.result();
    if (!path.empty() && path.size() < 4) {
        path += ".txt";
    }
    return path;
}

}

GuiApp::GuiApp()
    : assets(),
      boardRenderer(nullptr),
      font(GetFontDefault()),
      map(0, 0),
      result(),
      filename(),
      statusMsg(),
      statusOk(false),
      hasResult(false),
      filenameFocused(false),
      selectedAlgo("A*"),
      selectedHeuristic("H1"),
      currentStep(0),
      totalSteps(0),
      isPlaying(false),
      playTimer(0.0f),
      playbackActorPos(0, 0),
      playbackCollectedMask(0),
      running(true),
      screenWidth(1280),
      screenHeight(720) {}

GuiApp::~GuiApp() {
    shutdown();
}

bool GuiApp::init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Ice Sliding Puzzle Solver");
    if (!IsWindowReady()) return false;
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    font = LoadFontEx("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 64, nullptr, 0);
    if (font.baseSize == 0) {
        font = LoadFontEx("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 64, nullptr, 0);
    }
    if (font.baseSize == 0) {
        font = GetFontDefault();
    }

    assets.loadAll();
    boardRenderer = std::make_unique<BoardRenderer>(assets, font);
    return true;
}

void GuiApp::shutdown() {
    boardRenderer.reset();
    if (font.baseSize != 0 && font.texture.id != 0) {
        UnloadFont(font);
    }
    assets.unloadAll();
    if (IsWindowReady()) CloseWindow();
}

void GuiApp::run() {
    while (!WindowShouldClose() && running) {
        update();
        draw();
    }
}

void GuiApp::update() {
    if (isPlaying && hasResult && currentStep < totalSteps) {
        playTimer += GetFrameTime();
        if (playTimer > 0.4f) {
            playTimer = 0.0f;
            currentStep++;
            computePlaybackState();
        }
    }
}

void GuiApp::computePlaybackState() {
    playbackActorPos = map.getStartPosition();
    playbackCollectedMask = 0;

    const std::string& path = result.getSolutionPath();
    for (int i = 0; i < currentStep && i < static_cast<int>(path.length()); ++i) {
        State state(playbackActorPos, playbackCollectedMask, 0, "", 0, 0, nullptr);
        MoveResult move = MovementEngine::slide(map, state, path[i]);
        if (move.isValid()) {
            playbackActorPos = move.getNewPosition();
            playbackCollectedMask = move.getNewCollectedMask();
        }
    }
}

void GuiApp::draw() {
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

    drawLeftPanel();
    drawRightPanel();
    drawCenterPanel();
    EndDrawing();
}

void GuiApp::drawTextC(const char* text, int x, int y, int fontSize, Color color) {
    DrawTextEx(font, text, Vector2{(float)x, (float)y}, (float)fontSize, 1.0f, color);
}

void GuiApp::drawTextC(const std::string& text, int x, int y, int fontSize, Color color) {
    drawTextC(text.c_str(), x, y, fontSize, color);
}

int GuiApp::measureTextC(const char* text, int fontSize) {
    return static_cast<int>(MeasureTextEx(font, text, (float)fontSize, 1.0f).x);
}

int GuiApp::measureTextC(const std::string& text, int fontSize) {
    return measureTextC(text.c_str(), fontSize);
}

void GuiApp::drawPanel(int x, int y, int w, int h, Color fill, Color border) {
    DrawRectangle(x, y, w, h, fill);
    DrawRectangleLinesEx(Rectangle{(float)x, (float)y, (float)w, (float)h}, 2, border);
}

bool GuiApp::radioButton(int x, int y, const char* label, bool selected, int fontSize) {
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

bool GuiApp::buttonRect(int x, int y, int w, int h, const char* label) {
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

bool GuiApp::imageButton(int x, int y, int w, int h, const std::string& texName) {
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

void GuiApp::drawStat(int x, int y, const char* key, const char* val, Color valColor, int fontSize) {
    drawTextC(key, x, y, fontSize, LIGHTGRAY);
    int kw = measureTextC(key, fontSize);
    drawTextC(val, x + kw + 8, y, fontSize, valColor);
}

void GuiApp::drawLeftPanel() {
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

    if (radioButton(x, y, "A*", selectedAlgo == "A*", itemSize)) selectedAlgo = "A*";
    y += 30;
    if (radioButton(x, y, "UCS", selectedAlgo == "UCS", itemSize)) selectedAlgo = "UCS";
    y += 30;
    if (radioButton(x, y, "GBFS", selectedAlgo == "GBFS", itemSize)) selectedAlgo = "GBFS";
    y += headingSize + 25;

    drawTextC("HEURISTIC", x, y, headingSize, {180, 200, 230, 255});
    y += headingSize + 15;

    if (radioButton(x, y, "H1 - Basic", selectedHeuristic == "H1", itemSize)) selectedHeuristic = "H1";
    y += 30;
    if (radioButton(x, y, "H2 - Custom", selectedHeuristic == "H2", itemSize)) selectedHeuristic = "H2";
    y += 30;
    if (radioButton(x, y, "H3 - Advanced", selectedHeuristic == "H3", itemSize)) selectedHeuristic = "H3";
    y += headingSize + 25;

    int btnH = std::max(36, sh / 20);
    if (buttonRect(x, y, pw - 40, btnH, "SOLVE")) {
        solve();
    }
}

void GuiApp::drawRightPanel() {
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
    if (buttonRect(x, y, pw - 40, btnH, "LOAD FILE")) {
        std::string picked = pickFileOpen();
        if (!picked.empty()) {
            filename = picked;
            loadFile();
        }
    }
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

        drawStat(x, y, "Path:", result.getSolutionPath().c_str(), WHITE, itemSize);
        y += itemSize + 8;

        drawStat(x, y, "Cost:", std::to_string(result.getTotalCost()).c_str(), WHITE, itemSize);
        y += itemSize + 8;

        std::string timeStr = std::to_string(static_cast<int>(result.getExecutionTimeMs())) + " ms";
        drawStat(x, y, "Time:", timeStr.c_str(), WHITE, itemSize);
        y += itemSize + 8;

        drawStat(x, y, "Iterations:", std::to_string(result.getIterationsCount()).c_str(), WHITE, itemSize);
        y += itemSize + 8;

        if (!result.getMessage().empty()) {
            drawTextC(result.getMessage(), x, y, smallSize, GRAY);
            y += smallSize + 8;
        }

        y += 15;
        if (buttonRect(x, y, pw - 40, btnH, "SAVE AS...")) {
            std::string picked = pickFileSave();
            if (!picked.empty()) {
                saveResult(picked);
            }
        }
    } else {
        drawTextC("No result yet.", x, y, itemSize, GRAY);
    }
}

void GuiApp::drawCenterPanel() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int leftW = std::max(200, sw * 18 / 100);
    int rightW = std::max(240, sw * 22 / 100);
    int margin = 10;
    int x = leftW + margin * 2;
    int w = sw - leftW - rightW - margin * 4;
    int h = sh;

    Texture2D boardTex = assets.has("board") ? assets.get("board") : Texture2D{0,0,0,0,0};
    if (boardTex.id != 0) {
        DrawTexturePro(boardTex,
            Rectangle{0, 0, (float)boardTex.width, (float)boardTex.height},
            Rectangle{(float)x, (float)margin, (float)w, (float)(h - margin * 2 - 100)},
            Vector2{0, 0}, 0.0f, WHITE);
    }

    int boardH = h - margin * 2 - 100;
    if (map.getRows() > 0 && map.getCols() > 0) {
        boardRenderer->render(map, playbackActorPos, x, margin + 10, w, boardH - 20);
    } else {
        int msgSize = std::max(18, sh / 40);
        const char* msg = "Load a file to display the board";
        int tw = measureTextC(msg, msgSize);
        drawTextC(msg, x + (w - tw) / 2, h / 2 - msgSize / 2, msgSize, GRAY);
    }

    if (hasResult && result.isSolutionFound()) {
        drawPlaybackControls(x + w / 2, h - margin - 70);
    }
}

void GuiApp::drawPlaybackControls(int cx, int cy) {
    int btnW = 55;
    int btnH = 35;
    int gap = 12;
    int totalW = btnW * 5 + gap * 4;
    int sx = cx - totalW / 2;

    if (imageButton(sx, cy, btnW, btnH, "btnBackArrow")) {
        currentStep = 0;
        isPlaying = false;
        computePlaybackState();
    }
    sx += btnW + gap;

    if (imageButton(sx, cy, btnW, btnH, "btnPrev")) {
        if (currentStep > 0) currentStep--;
        isPlaying = false;
        computePlaybackState();
    }
    sx += btnW + gap;

    std::string playTex = isPlaying ? "btnPause" : "btnPlay";
    if (imageButton(sx, cy, btnW, btnH, playTex)) {
        isPlaying = !isPlaying;
        if (currentStep >= totalSteps && totalSteps > 0) {
            currentStep = 0;
            computePlaybackState();
        }
    }
    sx += btnW + gap;

    if (imageButton(sx, cy, btnW, btnH, "btnNext")) {
        if (currentStep < totalSteps) currentStep++;
        isPlaying = false;
        computePlaybackState();
    }
    sx += btnW + gap;

    if (imageButton(sx, cy, btnW, btnH, "btnNextArrow")) {
        currentStep = totalSteps;
        isPlaying = false;
        computePlaybackState();
    }

    int stepSize = std::max(14, GetScreenHeight() / 50);
    std::string stepStr = "Step: " + std::to_string(currentStep) + " / " + std::to_string(totalSteps);
    int stw = measureTextC(stepStr, stepSize);
    drawTextC(stepStr, cx - stw / 2, cy - stepSize - 8, stepSize, WHITE);
}

void GuiApp::loadFile() {
    if (filename.empty()) {
        statusMsg = "Filename is empty";
        statusOk = false;
        return;
    }
    try {
        parser p;
        if (!p.parseFile(filename)) {
            statusMsg = "Failed to parse file";
            statusOk = false;
            return;
        }
        GameMap newMap(p.getRows(), p.getCols());
        for (int i = 0; i < p.getRows(); ++i) {
            for (int j = 0; j < p.getCols(); ++j) {
                std::string tileStr = p.getGrid(i, j);
                char tile = tileStr.empty() ? '*' : tileStr[0];
                int cost = p.getPoint(i, j);
                newMap.setTile(i, j, tile);
                newMap.setCost(i, j, cost);
                if (tile == 'Z') newMap.setStartPosition(Position(i, j));
                if (tile == 'O') newMap.setGoalPosition(Position(i, j));
            }
        }
        InputValidator::validate(newMap);
        map = newMap;
        hasResult = false;
        currentStep = 0;
        totalSteps = 0;
        isPlaying = false;
        playbackActorPos = map.getStartPosition();
        playbackCollectedMask = 0;
        statusMsg = "Map loaded: " + std::to_string(map.getRows()) + "x" + std::to_string(map.getCols());
        statusOk = true;
    } catch (const GameException& e) {
        statusMsg = e.what();
        statusOk = false;
    }
}

void GuiApp::solve() {
    if (map.getRows() == 0) {
        statusMsg = "Load a file first";
        statusOk = false;
        return;
    }

    std::unique_ptr<ISolver> solver;
    std::unique_ptr<IHeuristic> heuristic;

    std::string h = toUpperStr(selectedHeuristic);
    if (h == "H1" || h == "H2") {
        heuristic = std::make_unique<Custom>();
    } else {
        heuristic = std::make_unique<Advanced>();
    }

    std::string algo = toUpperStr(selectedAlgo);
    if (algo == "A*" || algo == "ASTAR") {
        solver = std::make_unique<AStar>(heuristic.get());
    } else {
        solver = std::make_unique<AStar>(heuristic.get());
    }

    result = solver->solve(map);
    hasResult = true;
    currentStep = 0;
    totalSteps = static_cast<int>(result.getSolutionPath().length());
    isPlaying = false;
    computePlaybackState();

    statusMsg = result.isSolutionFound() ? "Solution found!" : "No solution found";
    statusOk = result.isSolutionFound();
}

void GuiApp::saveResult(const std::string& outPath) {
    if (!hasResult || outPath.empty()) return;
    try {
        SolutionSaver::save(result, map, selectedAlgo, selectedHeuristic, outPath);
        statusMsg = "Saved to " + outPath;
        statusOk = true;
    } catch (...) {
        statusMsg = "Failed to save";
        statusOk = false;
    }
}

#else

GuiApp::GuiApp() {}
GuiApp::~GuiApp() {}
bool GuiApp::init() { return false; }
void GuiApp::shutdown() {}
void GuiApp::run() {}

#endif
