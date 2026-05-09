#pragma once

#include "model/GameMap.hpp"
#include "model/Position.hpp"
#include "gui/AssetManager.hpp"
#include "gui/BoardRenderer.hpp"

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

#ifdef HAVE_RAYLIB

struct BoardViewPageInput {
    bool back = false;
    bool rewind = false;
    bool prev = false;
    bool playPause = false;
    bool next = false;
    bool end = false;
};

class BoardViewPage {
public:
    BoardViewPage(AssetManager& am, Font f);

    void beginFrame();
    void endFrame();

    BoardViewPageInput draw(const GameMap& map, const Position& actorPos,
                             int collectedMask, bool hasResult, bool isPlaying,
                             int currentStep, int totalSteps,
                             BoardRenderer& boardRenderer);

private:
    AssetManager& assets;
    Font font;

    bool buttonRect(int x, int y, int w, int h, const char* label);
    bool imageButton(int x, int y, int w, int h, const std::string& texName);
    void drawTextC(const char* text, int x, int y, int fontSize, Color color);
    void drawTextC(const std::string& text, int x, int y, int fontSize, Color color);
    int measureTextC(const char* text, int fontSize);
};

#endif
