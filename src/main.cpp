#include "utils/parser.hpp"

int main() {
    parser p;
    p.parseFile("test/test.txt");
    p.printBoards();
    return 0;
}