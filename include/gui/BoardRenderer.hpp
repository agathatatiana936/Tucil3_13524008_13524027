#pragma once

#include "model/GameMap.hpp"
#include "model/Position.hpp"

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

class AssetManager;

class BoardRenderer {
public:
#ifdef HAVE_RAYLIB
    explicit BoardRenderer(const AssetManager& am, Font f);
    void render(const GameMap& map, const Position& actorPos,
                int collectedMask,
                int x, int y, int maxW, int maxH) const;
#endif

private:
    const AssetManager& assets;
#ifdef HAVE_RAYLIB
    Font font;

    void drawTile(char tile, int x, int y, int size, int collectedMask) const;
    void drawActor(const Position& pos, int bx, int by, int size) const;
    void drawTextC(const char* text, int x, int y, int size, Color c) const;
    int measureW(const char* text, int size) const;
#endif
};
