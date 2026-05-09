#include "utils/InputValidator.hpp"
#include "exception/ValidationException.hpp"

void InputValidator::validate(const GameMap& map) {
    validateStartGoal(map);
    validateNumbers(map);
    validateSymbols(map);
}

void InputValidator::validateStartGoal(const GameMap& map) {
    int zCount = 0, oCount = 0;
    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            char t = map.getTile(i, j);
            if (t == 'Z') zCount++;
            if (t == 'O') oCount++;
        }
    }

    if (zCount != 1) {
        throw ValidationException("Harus ada tepat 1 aktor (Z).");
    }
    if (oCount != 1) {
        throw ValidationException("Harus ada tepat 1 tujuan (O).");
    }
}

void InputValidator::validateNumbers(const GameMap& map) {
    bool hasNumber[10] = {false};

    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            if (map.isNumbered(i, j)) {
                int num = map.getNumber(i, j);
                if (num < 0 || num > 9) {
                    throw ValidationException("Angka harus antara 0-9.");
                }
                if (hasNumber[num]) {
                    throw ValidationException("Angka " + std::to_string(num) + " duplikat.");
                }
                hasNumber[num] = true;
            }
        }
    }

    int maxNum = -1;
    for (int i = 0; i < 10; ++i) {
        if (hasNumber[i]) maxNum = i;
    }

    for (int i = 0; i <= maxNum; ++i) {
        if (!hasNumber[i]) {
            throw ValidationException("Angka harus berurutan dari 0. Angka " + std::to_string(i) + " tidak ditemukan.");
        }
    }
}

void InputValidator::validateSymbols(const GameMap& map) {
    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            char t = map.getTile(i, j);
            bool valid = (t == 'X' || t == '*' || t == 'L' || t == 'Z' || t == 'O' || (t >= '0' && t <= '9'));
            if (!valid) {
                throw ValidationException("Simbol tidak valid: " + std::string(1, t));
            }
        }
    }
}
