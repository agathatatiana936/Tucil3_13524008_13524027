#include "model/Position.hpp"

Position::Position() : row(0), col(0) {}

Position::Position(int row, int col) : row(row), col(col) {}

Position::Position(const Position& other) : row(other.row), col(other.col) {}

int Position::getRow() const { return row; }

int Position::getCol() const { return col; }

void Position::setRow(int row) { this->row = row; }

void Position::setCol(int col) { this->col = col; }

bool Position::operator==(const Position& other) const {
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

bool Position::operator<(const Position& other) const {
    if (row != other.row) return row < other.row;
    return col < other.col;
}

Position& Position::operator=(const Position& other) {
    if (this != &other) {
        row = other.row;
        col = other.col;
    }
    return *this;
}

namespace std {
    size_t hash<Position>::operator()(const Position& pos) const {
        return std::hash<int>()(pos.getRow()) ^ (std::hash<int>()(pos.getCol()) << 1);
    }
}
