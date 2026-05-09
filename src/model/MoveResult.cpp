#include "model/MoveResult.hpp"

MoveResult::MoveResult() 
    : valid(false), newPosition(), moveCost(0), gameOver(false),
      newCollectedMask(0), newLastCollected(-1) {}

MoveResult::MoveResult(bool valid, const Position& newPos, int cost, bool gameOver,
                       const std::string& reason, const std::vector<Position>& traversed,
                       int newCollectedMask, int newLastCollected)
    : valid(valid), newPosition(newPos), moveCost(cost), gameOver(gameOver),
      gameOverReason(reason), traversedTiles(traversed),
      newCollectedMask(newCollectedMask), newLastCollected(newLastCollected) {}

bool MoveResult::isValid() const { return valid; }

const Position& MoveResult::getNewPosition() const { return newPosition; }

int MoveResult::getMoveCost() const { return moveCost; }

bool MoveResult::isGameOver() const { return gameOver; }

const std::string& MoveResult::getGameOverReason() const { return gameOverReason; }

const std::vector<Position>& MoveResult::getTraversedTiles() const { return traversedTiles; }

int MoveResult::getNewCollectedMask() const { return newCollectedMask; }

int MoveResult::getNewLastCollected() const { return newLastCollected; }
