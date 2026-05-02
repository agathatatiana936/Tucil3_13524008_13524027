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
    for (const auto& b : boards) {
        std::cout << "Papan: " << b.getGrid() << ", Point: " << b.getPoint() << std::endl;
    }
}