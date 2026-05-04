#include "utils/SolutionSaver.hpp"
#include "utils/MovementEngine.hpp"
#include "model/State.hpp"
#include <fstream>
#include <iostream>

static void writeBoard(std::ofstream& file, const GameMap& map, const Position& actorPos, int collectedMask) {
    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            if (actorPos.getRow() == i && actorPos.getCol() == j) {
                file << 'Z';
            } else {
                char tile = map.getTile(i, j);
                if (tile >= '0' && tile <= '9') {
                    int num = tile - '0';
                    if ((collectedMask & (1 << num)) != 0) {
                        file << '*';
                    } else {
                        file << tile;
                    }
                } else if (tile == 'Z') {
                    file << '*';
                } else {
                    file << tile;
                }
            }
        }
        file << '\n';
    }
}

void SolutionSaver::save(const SearchResult& result, const GameMap& map,
                         const std::string& algorithm, const std::string& heuristic,
                         const std::string& filename) {
    std::string saveDir = "saves";
    std::ofstream file(saveDir + "/" + filename);
    if (!file.is_open()) {
        std::cout << "Error: Gagal membuka file '" << filename << "' untuk menulis.\n";
        return;
    }

    file << "Initial State:\n";
    writeBoard(file, map, map.getStartPosition(), 0);
    file << "\n";

    file << "Algoritma: " << algorithm << "\n";
    if (!heuristic.empty()) {
        file << "Heuristic: " << heuristic << "\n";
    }
    file << "\n";

    if (result.isSolutionFound()) {
        file << "Solusi Yang Ditemukan : " << result.getSolutionPath() << "\n";
        file << "Cost dari Solusi : " << result.getTotalCost() << "\n";
    } else {
        file << "Solusi Yang Ditemukan : Tidak ada solusi\n";
        file << "Cost dari Solusi : -\n";
    }
    file << "Waktu eksekusi : " << result.getExecutionTimeMs() << " ms\n";
    file << "Banyak iterasi : " << result.getIterationsCount() << " iterasi\n";

    if (!result.getMessage().empty()) {
        file << "Pesan : " << result.getMessage() << "\n";
    }

    file << "\n";

    if (result.isSolutionFound()) {
        file << "Langkah-langkah:\n";
        std::string path = result.getSolutionPath();
        Position currentPos = map.getStartPosition();
        State currentState(currentPos, 0, -1, "", 0, 0, nullptr);

        file << "Step 0 (Initial):\n";
        writeBoard(file, map, currentPos, 0);
        file << "\n";

        for (size_t i = 0; i < path.length(); ++i) {
            MoveResult move = MovementEngine::slide(map, currentState, path[i]);
            if (!move.isValid()) break;

            currentPos = move.getNewPosition();
            currentState = State(currentPos, move.getNewCollectedMask(),
                                move.getNewLastCollected(), "", 0, 0, nullptr);

            file << "Step " << (i + 1) << " : " << path[i] << "\n";
            writeBoard(file, map, currentPos, currentState.getCollectedMask());
            file << "\n";
        }
    }

    file.close();
    std::cout << ">> Solusi disimpan pada saves/" << filename << "\n\n";
}
