#include "gui/BoardRenderer.hpp"
#include "gui/AssetManager.hpp"

#ifdef HAVE_RAYLIB
#include <raylib.h>
#include <algorithm>
#endif

#ifdef HAVE_RAYLIB

BoardRenderer::BoardRenderer(const AssetManager& am, Font f)
    : assets(am), font(f) {}

void BoardRenderer::render(const GameMap& map, const Position& actorPos,
                           int collectedMask,
                           int x, int y, int maxW, int maxH,
                           int maxRows, int maxCols) const {
    if (map.getRows() == 0 || map.getCols() == 0) return;

    int refRows = map.getRows();
    int refCols = map.getCols();
    if (maxRows > 0 && map.getRows() < maxRows) refRows = maxRows;
    if (maxCols > 0 && map.getCols() < maxCols) refCols = maxCols;

    int tileSize = std::min(maxW / refCols, maxH / refRows);
    int maxBoardW = 64 * 6;
    tileSize = std::min(tileSize, 64);
    int boardW = map.getCols() * tileSize;
    int boardH = map.getRows() * tileSize;
    if(boardW > maxBoardW && map.getCols() < 11){
        boardW = maxBoardW;
        tileSize = boardW / map.getCols();
    }
    if(boardH > maxBoardW && map.getRows() < 11){
        boardH = maxBoardW;
        tileSize = boardH / map.getRows();
    }
    int bx = x + (maxW - boardW) / 2;
    int by = y + (maxH - boardH) / 2;

    for (int r = 0; r < map.getRows(); ++r) {
        for (int c = 0; c < map.getCols(); ++c) {
            int tx = bx + c * tileSize;
            int ty = by + r * tileSize;
            drawTile(map.getTile(r, c), tx, ty, tileSize, collectedMask);
        }
    }

    drawActor(actorPos, bx, by, tileSize);
}

void BoardRenderer::drawTile(char tile, int x, int y, int size, int collectedMask) const {
    bool isCollected = false;
    if (tile >= '0' && tile <= '9') {
        int number = tile - '0';
        isCollected = (collectedMask & (1 << number)) != 0;
    }

    char displayTile = tile;
    if (isCollected) {
        displayTile = '.';
    }

    Texture2D tex{0,0,0,0,0};
    switch (displayTile) {
        case '.':
        case 'Z':
            tex = assets.has("iceTile") ? assets.get("iceTile") : tex;
            break;
        case 'X':
            tex = assets.has("wallTile") ? assets.get("wallTile") : tex;
            break;
        case 'L':
            tex = assets.has("lavaTile") ? assets.get("lavaTile") : tex;
            break;
        case 'O':
            tex = assets.has("igloo") ? assets.get("igloo") : tex;
            break;
        default:
            tex = assets.has("iceTile") ? assets.get("iceTile") : tex;
            break;
    }

    if (tex.id != 0) {
        DrawTexturePro(tex,
            Rectangle{0, 0, (float)tex.width, (float)tex.height},
            Rectangle{(float)x, (float)y, (float)size, (float)size},
            Vector2{0, 0}, 0.0f, WHITE);
    } else {
        Color col = WHITE;
        switch (displayTile) {
            case '.': col = {200, 220, 240, 255}; break;
            case 'X': col = {80, 80, 80, 255}; break;
            case 'L': col = {100, 200, 100, 255}; break;
            case 'O': col = {255, 200, 50, 255}; break;
            case 'Z': col = {200, 200, 200, 255}; break;
            default:
                if (displayTile >= '0' && displayTile <= '9') col = {100, 150, 255, 255};
                break;
        }
        DrawRectangle(x, y, size, size, col);
    }

    if (!isCollected && tile >= '0' && tile <= '9') {
        std::string numName = "num";
        numName.push_back(tile);
        Texture2D numTex = assets.has(numName) ? assets.get(numName) : Texture2D{0,0,0,0,0};
        if (numTex.id != 0) {
            int pad = size / 4;
            DrawTexturePro(numTex,
                Rectangle{0, 0, (float)numTex.width, (float)numTex.height},
                Rectangle{(float)(x + pad), (float)(y + pad), (float)(size - pad*2), (float)(size - pad*2)},
                Vector2{0, 0}, 0.0f, WHITE);
        } else {
            int fs = size / 2;
            int tw = measureW("*", fs);
            drawTextC("*", x + (size - tw) / 2, y + (size - fs) / 2, fs, DARKGRAY);
        }
    }

    if (tile == 'X' || tile == 'L' || tile == 'O') {
        const char* label = nullptr;
        switch (tile) {
            case 'X': label = "X"; break;
            case 'L': break;
            case 'O': break;
        }
        if (label) {
            int fs = size / 2;
            int tw = measureW(label, fs);
            drawTextC(label, x + (size - tw) / 2, y + (size - fs) / 2, fs, DARKGRAY);
        }
    }

    DrawRectangleLines(x, y, size, size, {60, 70, 90, 255});
}

void BoardRenderer::drawActor(const Position& pos, int bx, int by, int size) const {
    int ax = bx + pos.getCol() * size;
    int ay = by + pos.getRow() * size;

    Texture2D actorTex = assets.has("actor") ? assets.get("actor") : Texture2D{0,0,0,0,0};
    if (actorTex.id != 0) {
        DrawTexturePro(actorTex,
            Rectangle{0, 0, (float)actorTex.width, (float)actorTex.height},
            Rectangle{(float)ax, (float)ay, (float)size, (float)size},
            Vector2{0, 0}, 0.0f, WHITE);
    } else {
        int cx = ax + size / 2;
        int cy = ay + size / 2;
        int radius = size / 3;
        DrawCircle(cx, cy, (float)radius, ORANGE);
        DrawCircleLines(cx, cy, (float)radius, DARKBROWN);
    }
}

void BoardRenderer::drawTextC(const char* text, int x, int y, int size, Color c) const {
    DrawTextEx(font, text, Vector2{(float)x, (float)y}, (float)size, 1.0f, c);
}

int BoardRenderer::measureW(const char* text, int size) const {
    return static_cast<int>(MeasureTextEx(font, text, (float)size, 1.0f).x);
}

#endif
