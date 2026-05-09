#pragma once

#include "exception/GameException.hpp"

class FileException : public GameException {
public:
    explicit FileException(const std::string& message);
};
