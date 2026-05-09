#include "exception/ValidationException.hpp"

ValidationException::ValidationException(const std::string& message) : GameException(message) {}
