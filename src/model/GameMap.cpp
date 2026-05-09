#include "model/GameMap.hpp"
#include <iostream>

GameMap::GameMap() : rows(0), cols(0), startPos(), goalPos() {}

GameMap::GameMap(int rows, int cols) 
    : rows(rows), cols(cols), grid(rows, std::vector<char>(cols, '*')), 
      costs(rows, std::vector<int>(cols, 0)), startPos(), goalPos() {}

GameMap::GameMap(const GameMap& other)
    : rows(other.rows), cols(other.cols), grid(other.grid), costs(other.costs),
      startPos(other.startPos), goalPos(other.goalPos) {}

int GameMap::getRows() const { return rows; }

int GameMap::getCols() const { return cols; }

char GameMap::getTile(int row, int col) const {
    if (!isInsideBounds(row, col)) return 'X';
    return grid[row][col];
}

char GameMap::getTile(const Position& pos) const {
    return getTile(pos.getRow(), pos.getCol());
}

int GameMap::getCost(int row, int col) const {
    if (!isInsideBounds(row, col)) return 0;
    return costs[row][col];
}

int GameMap::getCost(const Position& pos) const {
    return getCost(pos.getRow(), pos.getCol());
}

const Position& GameMap::getStartPosition() const { return startPos; }

const Position& GameMap::getGoalPosition() const { return goalPos; }

void GameMap::setTile(int row, int col, char tile) {
    if (isInsideBounds(row, col)) grid[row][col] = tile;
}

void GameMap::setCost(int row, int col, int cost) {
    if (isInsideBounds(row, col)) costs[row][col] = cost;
}

void GameMap::setStartPosition(const Position& pos) { startPos = pos; }

void GameMap::setGoalPosition(const Position& pos) { goalPos = pos; }

bool GameMap::isInsideBounds(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool GameMap::isInsideBounds(const Position& pos) const {
    return isInsideBounds(pos.getRow(), pos.getCol());
}

bool GameMap::isObstacle(int row, int col) const {
    return getTile(row, col) == 'X';
}

bool GameMap::isLava(int row, int col) const {
    return getTile(row, col) == 'L';
}

bool GameMap::isPassable(int row, int col) const {
    char t = getTile(row, col);
    return t != 'X';
}

bool GameMap::isNumbered(int row, int col) const {
    char t = getTile(row, col);
    return t >= '0' && t <= '9';
}

int GameMap::getNumber(int row, int col) const {
    if (isNumbered(row, col)) return getTile(row, col) - '0';
    return -1;
}

void GameMap::print() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << grid[i][j];
        }
        std::cout << '\n';
    }
}

GameMap GameMap::clone() const {
    return GameMap(*this);
}
