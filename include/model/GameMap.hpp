#pragma once

#include <vector>
#include <string>
#include "model/Position.hpp"

class GameMap {
private:
    int rows;
    int cols;
    std::vector<std::vector<char>> grid;
    std::vector<std::vector<int>> costs;
    Position startPos;
    Position goalPos;

public:
    GameMap();
    explicit GameMap(int rows, int cols);
    GameMap(const GameMap& other);
    GameMap& operator=(const GameMap& other) = default;
    ~GameMap() = default;

    int getRows() const;
    int getCols() const;
    char getTile(int row, int col) const;
    char getTile(const Position& pos) const;
    int getCost(int row, int col) const;
    int getCost(const Position& pos) const;

    const Position& getStartPosition() const;
    const Position& getGoalPosition() const;

    void setTile(int row, int col, char tile);
    void setCost(int row, int col, int cost);
    void setStartPosition(const Position& pos);
    void setGoalPosition(const Position& pos);

    bool isInsideBounds(int row, int col) const;
    bool isInsideBounds(const Position& pos) const;
    bool isObstacle(int row, int col) const;
    bool isLava(int row, int col) const;
    bool isPassable(int row, int col) const;
    bool isNumbered(int row, int col) const;
    int getNumber(int row, int col) const;

    void print() const;
    GameMap clone() const;
};
