#pragma once

#include "exception/GameException.hpp"

class SolverException : public GameException {
public:
    explicit SolverException(const std::string& message);
};
