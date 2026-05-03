#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

class board {
    private:
        char grid;
        int point;
    public:
        board(char grid, int point) : grid(grid), point(point) {}
        board(const board& other) = default;
        ~board() = default;
        char getGrid() const { return grid; }
        int getPoint() const { return point; }
        void fillGrid(char newGrid) { grid = newGrid; }
        void fillPoint(int newPoint) { point = newPoint; }
};

class parser{
    private :
        std::vector<board> boards;
        int rows;
        int cols;
    public :
        void parseFile(const std::string& filename);
        ~parser() = default;
        std::vector<board> getBoards() const;
        std::string getGrid(int row, int col) const;
        int getPoint(int row, int col) const;
        void printBoards() const;
};