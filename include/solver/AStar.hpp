#pragma once

#include "solver/ISolver.hpp"

class IHeuristic;
class State;
class GameMap;

class AStar : public ISolver {
public:
    explicit AStar(const IHeuristic* heuristic);
    SearchResult solve(const GameMap& map) const override;

private:
    const IHeuristic* heuristic;
};
