#pragma once

#include "model/SearchResult.hpp"

class GameMap;

class ISolver {
public:
    virtual ~ISolver() = default;
    virtual SearchResult solve(const GameMap& map) const = 0;
};
