#pragma once

#include "heuristic/IHeuristic.hpp"

class Manhattan : public IHeuristic {
public:
    int compute(const State& state, const GameMap& map) const override;
};
