#include "cli/CLI.hpp"
#include "utils/parser.hpp"
#include "utils/InputValidator.hpp"
#include "utils/SolutionSaver.hpp"
#include "utils/MovementEngine.hpp"
#include "exception/GameException.hpp"
#include "exception/FileException.hpp"
#include "exception/ValidationException.hpp"
#include "solver/AStar.hpp"
#include "heuristic/Custom.hpp"
#include "heuristic/Advanced.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <limits>

CLI::CLI() : running(true), currentMap(), lastResult(), currentFilename() {}

std::string CLI::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void CLI::clearScreen() {
    std::system("clear || cls");
}

void CLI::waitForEnter() {
    std::cout << "\nTekan Enter untuk melanjutkan...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void CLI::displayWelcome() const {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "    ICE SLIDING PUZZLE SOLVER\n";
    std::cout << "========================================\n";
    std::cout << "\n";
}

bool CLI::handleInputFile() {
    std::cout << ">> Masukan file input: ";
    std::string filename;
    std::getline(std::cin, filename);

    if (filename.empty()) {
        std::cout << "Error: Nama file tidak boleh kosong.\n";
        return false;
    }

    try {
        parser p;
        p.parseFile(filename);

        currentFilename = filename;
        currentMap = GameMap(p.getRows(), p.getCols());

        for (int i = 0; i < p.getRows(); ++i) {
            for (int j = 0; j < p.getCols(); ++j) {
                std::string tileStr = p.getGrid(i, j);
                char tile = tileStr.empty() ? '*' : tileStr[0];
                int cost = p.getPoint(i, j);
                currentMap.setTile(i, j, tile);
                currentMap.setCost(i, j, cost);
                if (tile == 'Z') currentMap.setStartPosition(Position(i, j));
                if (tile == 'O') currentMap.setGoalPosition(Position(i, j));
            }
        }

        InputValidator::validate(currentMap);

        std::cout << "\nPeta berhasil dimuat!\n";
        std::cout << "Ukuran peta: " << currentMap.getRows() << " x " << currentMap.getCols() << "\n\n";
        std::cout << "Initial State:\n";
        currentMap.print();
        std::cout << "\n";
        return true;
    } catch (const GameException& e) {
        std::cout << "Error: " << e.what() << "\n";
        return false;
    }
}

void CLI::handleAlgorithmSelection() {
    std::cout << ">> Algoritma apa yang anda pilih? (A*): ";
    std::string algo;
    std::getline(std::cin, algo);
    algo = toUpper(algo);

    std::unique_ptr<ISolver> solver;
    std::unique_ptr<IHeuristic> heuristic;

    selectedAlgorithm = "A*";

    if (algo == "A*" || algo == "ASTAR") {
        std::cout << ">> Heuristic apa yang anda pilih? (H1/H2/H3): ";
        std::string hChoice;
        std::getline(std::cin, hChoice);
        hChoice = toUpper(hChoice);

        if (hChoice == "H1") {
            selectedHeuristic = "H1";
            heuristic = std::make_unique<Custom>();
        } else if (hChoice == "H2") {
            selectedHeuristic = "H2";
            heuristic = std::make_unique<Custom>();
        } else if (hChoice == "H3") {
            selectedHeuristic = "H3";
            heuristic = std::make_unique<Custom>();
        } else {
            std::cout << "Heuristic tidak dikenal, menggunakan H1.\n";
            selectedHeuristic = "H1";
            heuristic = std::make_unique<Custom>();
        }

        solver = std::make_unique<AStar>(heuristic.get());
    } else {
        std::cout << "Algoritma tidak dikenal, menggunakan A*.\n";
        selectedHeuristic = "H1";
        heuristic = std::make_unique<Custom>();
        solver = std::make_unique<AStar>(heuristic.get());
    }

    std::cout << "\nSedang mencari solusi...\n\n";

    lastResult = solver->solve(currentMap);

    displayResult();

    std::cout << ">> Apakah Anda ingin melakukan playback? (Ya/Tidak): ";
    std::string pbChoice;
    std::getline(std::cin, pbChoice);
    if (toUpper(pbChoice) == "YA" || toUpper(pbChoice) == "Y") {
        handlePlayback();
    }

    std::cout << ">> Apakah Anda ingin menyimpan solusi? (Ya/Tidak): ";
    std::string saveChoice;
    std::getline(std::cin, saveChoice);
    if (toUpper(saveChoice) == "YA" || toUpper(saveChoice) == "Y") {
        handleSaveSolution();
    }
}

void CLI::displayResult() const {
    lastResult.printSummary();
    std::cout << "\n";
}

void CLI::handlePlayback() const {
    if (!lastResult.isSolutionFound()) {
        std::cout << "Tidak ada solusi untuk di-playback.\n";
        return;
    }

    const std::string& path = lastResult.getSolutionPath();
    int maxStep = static_cast<int>(path.length());

    bool keepPlaying = true;
    while (keepPlaying) {
        std::cout << ">> Pada step berapa anda ingin melakukan playback (0-" << maxStep << "): ";
        int step;
        std::cin >> step;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (step < 0 || step > maxStep) {
            std::cout << "Error: Step tidak valid. Range: 0-" << maxStep << "\n";
        } else {
            Position actorPos = currentMap.getStartPosition();
            int collectedMask = 0;
            int lastCollected = -1;

            for (int i = 0; i < step; ++i) {
                State state(actorPos, collectedMask, lastCollected, "", 0, 0, nullptr);
                MoveResult move = MovementEngine::slide(currentMap, state, path[i]);
                if (!move.isValid()) break;
                actorPos = move.getNewPosition();
                collectedMask = move.getNewCollectedMask();
                lastCollected = move.getNewLastCollected();
            }

            std::cout << "\n=== Playback Step " << step << " ===\n";
            if (step > 0) {
                std::cout << "Gerakan: " << path.substr(0, step) << "\n";
            }
            printBoard(currentMap, actorPos, collectedMask);
            std::cout << "\n";
        }

        std::cout << ">> Lanjutkan playback step lain? (Ya/Tidak): ";
        std::string again;
        std::getline(std::cin, again);
        keepPlaying = (toUpper(again) == "YA" || toUpper(again) == "Y");
    }
}

void CLI::handleSaveSolution() const {
    std::cout << ">> Masukkan nama file untuk menyimpan solusi: ";
    std::string outPath;
    std::getline(std::cin, outPath);

    if (outPath.empty()) {
        std::cout << "Error: Nama file tidak boleh kosong.\n";
        return;
    }

    if (outPath.length() < 4 || outPath.substr(outPath.length() - 4) != ".txt") {
        outPath += ".txt";
    }

    SolutionSaver::save(lastResult, currentMap, selectedAlgorithm, selectedHeuristic, outPath);
}

void CLI::printBoard(const GameMap& map, const Position& actorPos, int collectedMask) const {
    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            if (actorPos.getRow() == i && actorPos.getCol() == j) {
                std::cout << 'Z';
            } else {
                char tile = map.getTile(i, j);
                if (tile >= '0' && tile <= '9') {
                    int num = tile - '0';
                    if ((collectedMask & (1 << num)) != 0) {
                        std::cout << '*';
                    } else {
                        std::cout << tile;
                    }
                } else {
                    std::cout << tile;
                }
            }
        }
        std::cout << '\n';
    }
}

void CLI::run() {
    while (running) {
        displayWelcome();

        if (!handleInputFile()) {
            std::cout << "Coba lagi? (Ya/Tidak): ";
            std::string retry;
            std::getline(std::cin, retry);
            if (toUpper(retry) != "YA" && toUpper(retry) != "Y") {
                running = false;
            }
            continue;
        }

        handleAlgorithmSelection();

        std::cout << ">> Apakah Anda ingin memproses file lain? (Ya/Tidak): ";
        std::string again;
        std::getline(std::cin, again);
        if (toUpper(again) != "YA" && toUpper(again) != "Y") {
            running = false;
        }
    }

    std::cout << "\nTerima kasih telah menggunakan Ice Sliding Puzzle Solver!\n";
}
