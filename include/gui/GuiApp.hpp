#pragma once

#include "model/GameMap.hpp"
#include "model/SearchResult.hpp"
#include "gui/AssetManager.hpp"
#include "gui/BoardRenderer.hpp"
#include <memory>
#include <string>

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

class GuiApp {
public:
    GuiApp();
    ~GuiApp();

    bool init();
    void run();
    void shutdown();

private:
#ifdef HAVE_RAYLIB
    void update();
    void draw();
    void drawLeftPanel();
    void drawRightPanel();
    void drawCenterPanel();
    void drawPlaybackControls(int cx, int cy);

    void loadFile();
    void solve();
    void saveResult(const std::string& outPath);
    void computePlaybackState();

    bool radioButton(int x, int y, const char* label, bool selected, int fontSize);
    bool buttonRect(int x, int y, int w, int h, const char* label);
    bool imageButton(int x, int y, int w, int h, const std::string& texName);
    void drawPanel(int x, int y, int w, int h, Color fill, Color border);
    void drawStat(int x, int y, const char* key, const char* val, Color valColor, int fontSize);

    void drawTextC(const char* text, int x, int y, int fontSize, Color color);
    void drawTextC(const std::string& text, int x, int y, int fontSize, Color color);
    int measureTextC(const char* text, int fontSize);
    int measureTextC(const std::string& text, int fontSize);

    AssetManager assets;
    std::unique_ptr<BoardRenderer> boardRenderer;
    Font font;

    GameMap map;
    SearchResult result;

    std::string filename;
    std::string statusMsg;
    bool statusOk;
    bool hasResult;
    bool filenameFocused;

    std::string selectedAlgo;
    std::string selectedHeuristic;

    int currentStep;
    int totalSteps;
    bool isPlaying;
    float playTimer;

    Position playbackActorPos;
    int playbackCollectedMask;

    bool running;
    int screenWidth;
    int screenHeight;
#endif
};
