#include "solver/AStar.hpp"
#include "model/State.hpp"
#include "model/GameMap.hpp"
#include "model/SearchResult.hpp"
#include "heuristic/IHeuristic.hpp"
#include "utils/MovementEngine.hpp"
#include <iostream>
#include <queue>
#include <map>
#include <tuple>
#include <chrono>

AStar::AStar(const IHeuristic* heuristic) : heuristic(heuristic) {}

SearchResult AStar::solve(const GameMap& map) const {
    auto startTime = std::chrono::high_resolution_clock::now();

    int maxNumber = -1;
    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            if (map.isNumbered(i, j)) {
                maxNumber = std::max(maxNumber, map.getNumber(i, j));
            }
        }
    }

    auto cmp = [](const State& a, const State& b) {
        return a.getFCost() > b.getFCost();
    };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> openSet(cmp);

    std::map<std::tuple<int, int, int>, int> bestCost;

    Position startPos = map.getStartPosition();
    State startState(startPos, 0, -1, "", 0, 0, nullptr);
    startState.setHCost(heuristic->compute(startState, map));
    openSet.push(startState);

    int iterations = 0;

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();
        iterations++;

        auto key = std::make_tuple(current.getPosition().getRow(),
                                   current.getPosition().getCol(),
                                   current.getCollectedMask());
        auto it = bestCost.find(key);
        if (it != bestCost.end() && it->second < current.getGCost()) {
            continue;
        }

        bool isGoal = current.getPosition() == map.getGoalPosition();
        bool allNumbersCollected = (maxNumber == -1) || (current.getLastCollected() == maxNumber);

        if (isGoal && allNumbersCollected) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();

            SearchResult result;
            result.setSolutionFound(true);
            result.setSolutionPath(current.getPath());
            result.setTotalCost(current.getGCost());
            result.setExecutionTimeMs(elapsed);
            result.setIterationsCount(iterations);
            return result;
        }

        const char directions[4] = {'U', 'D', 'L', 'R'};
        for (int i = 0; i < 4; ++i) {
            MoveResult move = MovementEngine::slide(map, current, directions[i]);
            if (!move.isValid() || move.isGameOver()) {
                continue;
            }

            std::string newPath = current.getPath() + directions[i];
            int newGCost = current.getGCost() + move.getMoveCost();

            State nextState(move.getNewPosition(),
                           move.getNewCollectedMask(),
                           move.getNewLastCollected(),
                           newPath,
                           newGCost,
                           0,
                           nullptr);
            nextState.setHCost(heuristic->compute(nextState, map));

            auto nextKey = std::make_tuple(nextState.getPosition().getRow(),
                                           nextState.getPosition().getCol(),
                                           nextState.getCollectedMask());
            auto nextIt = bestCost.find(nextKey);
            if (nextIt != bestCost.end() && nextIt->second <= newGCost) {
                continue;
            }

            bestCost[nextKey] = newGCost;
            openSet.push(nextState);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    SearchResult result;
    result.setSolutionFound(false);
    result.setMessage("Solusi tidak ditemukan.");
    result.setExecutionTimeMs(elapsed);
    result.setIterationsCount(iterations);
    return result;
}
