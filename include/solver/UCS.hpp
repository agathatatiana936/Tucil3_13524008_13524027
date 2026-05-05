#pragma once

#include "solver/ISolver.hpp"

class GameMap;

class UCS : public ISolver {
public:
    UCS() = default;
    SearchResult solve(const GameMap& map) const override;
};
