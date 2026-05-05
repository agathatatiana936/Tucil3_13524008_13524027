#include "heuristic/Advanced.hpp"
#include "model/State.hpp"
#include "model/GameMap.hpp"
#include <vector>
#include <cstdlib>

int Advanced::compute(const State& state, const GameMap& map) const {
    std::vector<Position> targets;

    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            if (map.isNumbered(i, j)) {
                int num = map.getNumber(i, j);
                if (!state.hasCollected(num)) {
                    targets.push_back(Position(i, j));
                }
            }
        }
    }

    targets.push_back(map.getGoalPosition());

    Position current = state.getPosition();
    int total = 0;

    while (!targets.empty()) {
        size_t closestIdx = 0;
        int closestDist = std::abs(current.getRow() - targets[0].getRow())
                        + std::abs(current.getCol() - targets[0].getCol());

        for (size_t i = 1; i < targets.size(); ++i) {
            int dist = std::abs(current.getRow() - targets[i].getRow())
                     + std::abs(current.getCol() - targets[i].getCol());
            if (dist < closestDist) {
                closestDist = dist;
                closestIdx = i;
            }
        }

        total += closestDist;
        current = targets[closestIdx];
        targets.erase(targets.begin() + static_cast<std::ptrdiff_t>(closestIdx));
    }

    return total;
}
