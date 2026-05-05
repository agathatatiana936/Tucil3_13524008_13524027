#pragma once

#include "solver/ISolver.hpp"

class IHeuristic;
class GameMap;

class GBFS : public ISolver {
public:
    explicit GBFS(const IHeuristic* heuristic);
    SearchResult solve(const GameMap& map) const override;

private:
    const IHeuristic* heuristic;
};
