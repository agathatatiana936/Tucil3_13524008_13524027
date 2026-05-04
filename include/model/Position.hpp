#pragma once

#include <functional>

class Position {
private:
    int row;
    int col;

public:
    Position();
    Position(int row, int col);
    Position(const Position& other);
    ~Position() = default;

    int getRow() const;
    int getCol() const;
    void setRow(int row);
    void setCol(int col);

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    bool operator<(const Position& other) const;

    Position& operator=(const Position& other);
};

namespace std {
    template<>
    struct hash<Position> {
        size_t operator()(const Position& pos) const;
    };
}
