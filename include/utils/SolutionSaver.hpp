#pragma once

#include <string>
#include "model/SearchResult.hpp"
#include "model/GameMap.hpp"

class SolutionSaver {
public:
    static void save(const SearchResult& result, const GameMap& map,
                     const std::string& algorithm, const std::string& heuristic,
                     const std::string& filename);
};
