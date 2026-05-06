#pragma once

#include "solver/ISolver.hpp"

class IHeuristic;
class State;
class GameMap;

class WeightedAStar : public ISolver {
public:
    explicit WeightedAStar(const IHeuristic* heuristic);
    SearchResult solve(const GameMap& map) const override;

private:
    const IHeuristic* heuristic;
};
