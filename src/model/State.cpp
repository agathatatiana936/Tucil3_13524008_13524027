#include "model/State.hpp"

State::State(const Position& pos, int collectedMask, int lastCollected,
             const std::string& path, int gCost, int hCost,
             std::shared_ptr<State> parent)
    : position(pos), collectedMask(collectedMask), lastCollected(lastCollected),
      path(path), gCost(gCost), hCost(hCost), parent(parent) {}

State::State(const State& other)
    : position(other.position), collectedMask(other.collectedMask),
      lastCollected(other.lastCollected), path(other.path),
      gCost(other.gCost), hCost(other.hCost), parent(other.parent) {}

const Position& State::getPosition() const { return position; }

int State::getCollectedMask() const { return collectedMask; }

int State::getLastCollected() const { return lastCollected; }

const std::string& State::getPath() const { return path; }

int State::getGCost() const { return gCost; }

int State::getHCost() const { return hCost; }

int State::getFCost() const { return gCost + hCost; }

const std::shared_ptr<State>& State::getParent() const { return parent; }

void State::setHCost(int h) { hCost = h; }

bool State::hasCollected(int number) const {
    if (number < 0 || number > 9) return false;
    return (collectedMask & (1 << number)) != 0;
}

bool State::canCollect(int number) const {
    return number == lastCollected + 1;
}

bool State::operator==(const State& other) const {
    return position == other.position && collectedMask == other.collectedMask;
}

bool State::operator<(const State& other) const {
    if (gCost != other.gCost) return gCost < other.gCost;
    if (hCost != other.hCost) return hCost < other.hCost;
    return collectedMask < other.collectedMask;
}

namespace std {
    size_t hash<State>::operator()(const State& state) const {
        size_t h1 = std::hash<Position>()(state.getPosition());
        size_t h2 = std::hash<int>()(state.getCollectedMask());
        return h1 ^ (h2 << 1);
    }
}
