#pragma once

#include "model/Position.hpp"
#include <vector>
#include <string>

class MoveResult {
private:
    bool valid;
    Position newPosition;
    int moveCost;
    bool gameOver;
    std::string gameOverReason;
    std::vector<Position> traversedTiles;
    int newCollectedMask;
    int newLastCollected;

public:
    MoveResult();
    MoveResult(bool valid, const Position& newPos, int cost, bool gameOver,
               const std::string& reason, const std::vector<Position>& traversed,
               int newCollectedMask, int newLastCollected);
    ~MoveResult() = default;

    bool isValid() const;
    const Position& getNewPosition() const;
    int getMoveCost() const;
    bool isGameOver() const;
    const std::string& getGameOverReason() const;
    const std::vector<Position>& getTraversedTiles() const;
    int getNewCollectedMask() const;
    int getNewLastCollected() const;
};
