#pragma once

#include "model/GameMap.hpp"
#include "model/State.hpp"
#include "model/MoveResult.hpp"

class MovementEngine {
public:
    static MoveResult slide(const GameMap& map, const State& state, char dir);
};
