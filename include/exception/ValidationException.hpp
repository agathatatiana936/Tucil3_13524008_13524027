#pragma once

#include "exception/GameException.hpp"

class ValidationException : public GameException {
public:
    explicit ValidationException(const std::string& message);
};
