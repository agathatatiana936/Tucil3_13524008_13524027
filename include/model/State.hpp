#pragma once

#include "model/Position.hpp"
#include <memory>
#include <string>

class GameMap;

class State {
private:
    Position position;
    int collectedMask;
    int lastCollected;
    std::string path;
    int gCost;
    int hCost;
    std::shared_ptr<State> parent;

public:
    State(const Position& pos, int collectedMask, int lastCollected,
          const std::string& path, int gCost, int hCost,
          std::shared_ptr<State> parent);
    State(const State& other);
    State& operator=(const State& other) = default;
    ~State() = default;

    const Position& getPosition() const;
    int getCollectedMask() const;
    int getLastCollected() const;
    const std::string& getPath() const;
    int getGCost() const;
    int getHCost() const;
    int getFCost() const;
    const std::shared_ptr<State>& getParent() const;

    void setHCost(int h);
    bool hasCollected(int number) const;
    bool canCollect(int number) const;

    bool operator==(const State& other) const;
    bool operator<(const State& other) const;
};

namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State& state) const;
    };
}
