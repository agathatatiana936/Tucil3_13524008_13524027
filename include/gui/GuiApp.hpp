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

class GuiRenderer;

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
    void computePlaybackState();

    void loadFile();
    void solve();
    void saveResult(const std::string& outPath);

    AssetManager assets;
    std::unique_ptr<BoardRenderer> boardRenderer;
    std::unique_ptr<GuiRenderer> renderer;
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
