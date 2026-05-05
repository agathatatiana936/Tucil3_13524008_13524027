#include "heuristic/Custom.hpp"
#include "model/State.hpp"
#include "model/GameMap.hpp"
#include <cstdlib>

int Custom::compute(const State& state, const GameMap& map) const {
    int nextNumber = state.getLastCollected() + 1;
    Position target = map.getGoalPosition();
    bool foundTarget = false;

    for (int i = 0; i < map.getRows() && !foundTarget; ++i) {
        for (int j = 0; j < map.getCols() && !foundTarget; ++j) {
            if (map.isNumbered(i, j) && map.getNumber(i, j) == nextNumber) {
                if (!state.hasCollected(nextNumber)) {
                    target = Position(i, j);
                    foundTarget = true;
                }
            }
        }
    }

    int dr = std::abs(state.getPosition().getRow() - target.getRow());
    int dc = std::abs(state.getPosition().getCol() - target.getCol());
    return dr + dc;
}
