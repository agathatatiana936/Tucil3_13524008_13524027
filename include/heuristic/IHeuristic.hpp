#pragma once

class State;
class GameMap;

class IHeuristic {
public:
    virtual ~IHeuristic() = default;
    virtual int compute(const State& state, const GameMap& map) const = 0;
};
