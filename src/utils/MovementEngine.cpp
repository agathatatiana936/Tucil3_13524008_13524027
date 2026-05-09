#include "utils/MovementEngine.hpp"

static void getDelta(char dir, int& dr, int& dc) {
    dr = 0; dc = 0;
    switch (dir) {
        case 'U': dr = -1; break;
        case 'D': dr = 1; break;
        case 'L': dc = -1; break;
        case 'R': dc = 1; break;
    }
}

MoveResult MovementEngine::slide(const GameMap& map, const State& state, char dir) {
    int dr, dc;
    getDelta(dir, dr, dc);

    Position current = state.getPosition();
    int cost = 0;
    std::vector<Position> traversed;
    int newMask = state.getCollectedMask();
    int newLast = state.getLastCollected();

    while (true) {
        int nextRow = current.getRow() + dr;
        int nextCol = current.getCol() + dc;

        if (!map.isInsideBounds(nextRow, nextCol)) {
            return MoveResult(false, current, cost, true,
                              "Keluar batas papan",
                              traversed, newMask, newLast);
        }

        char tile = map.getTile(nextRow, nextCol);

        if (tile == 'X') {
            break;
        }

        if (tile == 'L') {
            return MoveResult(false, current, cost, true,
                              "Melewati lava",
                              traversed, newMask, newLast);
        }

        cost += map.getCost(nextRow, nextCol);
        current = Position(nextRow, nextCol);
        traversed.push_back(current);

        if (tile >= '0' && tile <= '9') {
            int number = tile - '0';
            if (!state.hasCollected(number)) {
                if (number == newLast + 1) {
                    newMask |= (1 << number);
                    newLast = number;
                } else {
                    return MoveResult(false, current, cost, true,
                                      "Angka tidak sesuai urutan",
                                      traversed, newMask, newLast);
                }
            }
        }
    }

    return MoveResult(true, current, cost, false, "",
                      traversed, newMask, newLast);
}
