#pragma once

#include "model/GameMap.hpp"

class InputValidator {
public:
    static void validate(const GameMap& map);

private:
    static void validateStartGoal(const GameMap& map);
    static void validateNumbers(const GameMap& map);
    static void validateSymbols(const GameMap& map);
};
