#include "exception/GameException.hpp"

GameException::GameException(const std::string& message) : message(message) {}

const char* GameException::what() const noexcept {
    return message.c_str();
}
