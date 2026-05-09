#pragma once

#include <string>
#include "model/GameMap.hpp"
#include "model/SearchResult.hpp"

class ISolver;

class CLI {
private:
    bool running;
    GameMap currentMap;
    SearchResult lastResult;
    std::string currentFilename;
    std::string selectedAlgorithm;
    std::string selectedHeuristic;

public:
    CLI();
    void run();

private:
    void displayWelcome() const;
    bool handleInputFile();
    void handleAlgorithmSelection();
    void displayResult() const;
    void handlePlayback() const;
    void handleSaveSolution() const;
    void printBoard(const GameMap& map, const Position& actorPos, int collectedMask = 0) const;
    static std::string toUpper(const std::string& str);
    static void clearScreen();
    static void waitForEnter();
};
