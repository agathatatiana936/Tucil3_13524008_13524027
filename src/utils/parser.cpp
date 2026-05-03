#include "utils/parser.hpp"
#include <vector>

void parser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    try{
        if (!file.is_open()) {
            throw std::runtime_error("File tidak ditemukan: " + filename);
            return;
        }

        int N, M;
        if (!(file >> N >> M)) {
            throw std::invalid_argument("Format N atau M pada baris pertama salah (bukan angka).");
        }
        rows = N;
        cols = M;

        std::vector<char> papans;
        std::vector<int> points;
        
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                char papan;
                if (!(file >> papan)) {
                    throw std::runtime_error("Papan tidak lengkap pada baris " + std::to_string(i));
                }
                papans.push_back(papan);
            }
        }

        for (int i = 0; i < N; i++){
            for (int j = 0; j < M; j++){
                int point;
                if (!(file >> point)) {
                    throw std::runtime_error("Point tidak lengkap pada baris " + std::to_string(i));
                }
                points.push_back(point);
            }
        }
        
        for (size_t k = 0; k < papans.size() && k < points.size(); ++k) {
            boards.push_back(board(papans[k], points[k]));
        }
        
        
    } catch (const std::exception& e) {
        std::cerr << "Error Terdeteksi: " << e.what() << std::endl;
        if (file.is_open()) file.close();
        return;
    }
    file.close();
}


void parser::printBoards() const {
    std::cout << getRows() << " " << getCols() << std::endl;
    for (int i = 0; i < getRows(); ++i) {
        for (int j = 0; j < getCols(); ++j) {
            std::cout << getGrid(i, j);
        }
        std::cout << std::endl;
    }
    for(int i = 0; i < getRows(); ++i) {
        for (int j = 0; j < getCols(); ++j) {
            std::cout << getPoint(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

std::string parser::getGrid(int row, int col) const {
    int index = row * cols + col;
    if (index >= 0 && static_cast<std::size_t>(index) < boards.size()) {
        return std::string(1, boards[index].getGrid());
    }
    return "";
}

int parser::getPoint(int row, int col) const {
    int index = row * cols + col;
    if (index >= 0 && static_cast<std::size_t>(index) < boards.size()) {
        return boards[index].getPoint();
    }
    return -1;
}

int parser::getRows() const {
    return rows;
}

int parser::getCols() const {
    return cols;
}