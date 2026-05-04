#include "heuristic/Custom.hpp"
#include "model/State.hpp"
#include "model/GameMap.hpp"

int Custom::compute(const State& state, const GameMap& map) const {
    int dr = std::abs(state.getPosition().getRow() - map.getGoalPosition().getRow());
    int dc = std::abs(state.getPosition().getCol() - map.getGoalPosition().getCol());
    return dr + dc;
}
