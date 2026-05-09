#pragma once

#include "heuristic/IHeuristic.hpp"

class Custom : public IHeuristic {
public:
    int compute(const State& state, const GameMap& map) const override;
};
