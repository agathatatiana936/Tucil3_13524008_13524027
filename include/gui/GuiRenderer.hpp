#pragma once

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif
#include <string>

class AssetManager;
class GameMap;
class Position;
class SearchResult;

#ifdef HAVE_RAYLIB

struct LeftPanelInput {
    bool algoAStar = false;
    bool algoUCS = false;
    bool algoGBFS = false;
    bool heuristicH1 = false;
    bool heuristicH2 = false;
    bool heuristicH3 = false;
    bool solveClicked = false;
};

struct RightPanelInput {
    Rectangle inputBoxRect{0,0,0,0};
    bool loadClicked = false;
    bool saveClicked = false;
};

struct CenterPanelInput {
    bool rewind = false;
    bool prev = false;
    bool playPause = false;
    bool next = false;
    bool end = false;
};

class GuiRenderer {
public:
    GuiRenderer(AssetManager& am, Font f);

    void beginFrame();
    void endFrame();

    LeftPanelInput drawLeftPanel(const std::string& selectedAlgo,
                                  const std::string& selectedHeuristic);
    RightPanelInput drawRightPanel(const std::string& filename,
                                    const std::string& statusMsg,
                                    bool statusOk,
                                    bool hasResult,
                                    const SearchResult& result);
    CenterPanelInput drawCenterPanel(const GameMap& map,
                                      const Position& actorPos,
                                      int collectedMask,
                                      bool hasResult,
                                      bool isPlaying,
                                      int currentStep,
                                      int totalSteps);

private:
    AssetManager& assets;
    Font font;

    bool radioButton(int x, int y, const char* label, bool selected, int fontSize);
    bool buttonRect(int x, int y, int w, int h, const char* label);
    bool imageButton(int x, int y, int w, int h, const std::string& texName);
    void drawPanel(int x, int y, int w, int h, Color fill, Color border);
    void drawStat(int x, int y, const char* key, const char* val,
                  Color valColor, int fontSize);
    void drawTextC(const char* text, int x, int y, int fontSize, Color color);
    void drawTextC(const std::string& text, int x, int y, int fontSize, Color color);
    int measureTextC(const char* text, int fontSize);
    int measureTextC(const std::string& text, int fontSize);
};

#endif
