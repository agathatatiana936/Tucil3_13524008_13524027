#include "gui/GuiApp.hpp"
#include "gui/GuiRenderer.hpp"
#include "gui/portable-file-dialogs.h"
#include "utils/parser.hpp"
#include "utils/InputValidator.hpp"
#include "utils/SolutionSaver.hpp"
#include "utils/MovementEngine.hpp"
#include "solver/AStar.hpp"
#include "solver/GBFS.hpp"
#include "solver/UCS.hpp"
#include "heuristic/Manhattan.hpp"
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
      renderer(nullptr),
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
    renderer = std::make_unique<GuiRenderer>(assets, font);
    return true;
}

void GuiApp::shutdown() {
    boardRenderer.reset();
    renderer.reset();
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

    if (filenameFocused) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) filename.push_back((char)key);
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !filename.empty()) filename.pop_back();
        if (IsKeyPressed(KEY_ENTER)) loadFile();
    }
}

void GuiApp::computePlaybackState() {
    playbackActorPos = map.getStartPosition();
    playbackCollectedMask = 0;
    int playbackLastCollected = -1;

    const std::string& path = result.getSolutionPath();
    for (int i = 0; i < currentStep && i < static_cast<int>(path.length()); ++i) {
        State state(playbackActorPos, playbackCollectedMask, playbackLastCollected, "", 0, 0, nullptr);
        MoveResult move = MovementEngine::slide(map, state, path[i]);
        if (move.isValid()) {
            playbackActorPos = move.getNewPosition();
            playbackCollectedMask = move.getNewCollectedMask();
            playbackLastCollected = move.getNewLastCollected();
        }
    }
}

void GuiApp::draw() {
    renderer->beginFrame();

    auto left = renderer->drawLeftPanel(selectedAlgo, selectedHeuristic);
    if (left.algoAStar) selectedAlgo = "A*";
    if (left.algoUCS) selectedAlgo = "UCS";
    if (left.algoGBFS) selectedAlgo = "GBFS";
    if (left.heuristicH1) selectedHeuristic = "H1";
    if (left.heuristicH2) selectedHeuristic = "H2";
    if (left.heuristicH3) selectedHeuristic = "H3";
    if (left.solveClicked) solve();

    std::string displayFilename = filename;
    size_t lastSep = displayFilename.find_last_of("/\\");
    if (lastSep != std::string::npos) {
        displayFilename = displayFilename.substr(lastSep + 1);
    }

    auto right = renderer->drawRightPanel(displayFilename, statusMsg, statusOk,
                                           hasResult, result);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        filenameFocused = false;
    }
    if (right.loadClicked) {
        std::string picked = pickFileOpen();
        if (!picked.empty()) {
            filename = picked;
            loadFile();
        }
    }
    if (right.saveClicked) {
        std::string picked = pickFileSave();
        if (!picked.empty()) {
            saveResult(picked);
        }
    }

    auto center = renderer->drawCenterPanel(map, playbackActorPos, playbackCollectedMask,
                                             hasResult, isPlaying, currentStep, totalSteps);
    if (center.rewind) {
        currentStep = 0; isPlaying = false; computePlaybackState();
    }
    if (center.prev) {
        if (currentStep > 0) currentStep--;
        isPlaying = false; computePlaybackState();
    }
    if (center.playPause) {
        isPlaying = !isPlaying;
        if (currentStep >= totalSteps && totalSteps > 0) {
            currentStep = 0; computePlaybackState();
        }
    }
    if (center.next) {
        if (currentStep < totalSteps) currentStep++;
        isPlaying = false; computePlaybackState();
    }
    if (center.end) {
        currentStep = totalSteps; isPlaying = false; computePlaybackState();
    }

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int leftW = std::max(200, sw * 18 / 100);
    int rightW = std::max(240, sw * 22 / 100);
    int margin = 10;
    int x = leftW + margin * 2;
    int w = sw - leftW - rightW - margin * 4;
    int h = sh;
    int boardH = h - margin * 2 - 100;
    if (map.getRows() > 0 && map.getCols() > 0) {
        boardRenderer->render(map, playbackActorPos, playbackCollectedMask,
                              x, margin + 10, w, boardH - 20);
    }

    renderer->endFrame();
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
    if (h == "H1") {
        heuristic = std::make_unique<Manhattan>();
    } else if (h == "H2") {
        heuristic = std::make_unique<Custom>();
    } else {
        heuristic = std::make_unique<Advanced>();
    }

    std::string algo = toUpperStr(selectedAlgo);
    if (algo == "A*" || algo == "ASTAR") {
        solver = std::make_unique<AStar>(heuristic.get());
    } else if (algo == "UCS") {
        solver = std::make_unique<UCS>();
    } else if (algo == "GBFS") {
        solver = std::make_unique<GBFS>(heuristic.get());
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
