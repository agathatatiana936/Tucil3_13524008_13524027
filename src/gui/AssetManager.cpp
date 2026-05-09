#include "gui/AssetManager.hpp"

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif

AssetManager::AssetManager() {}

AssetManager::~AssetManager() {
    unloadAll();
}

void AssetManager::loadAll() {
#ifdef HAVE_RAYLIB
    struct Def { const char* name; const char* path; };
    Def defs[] = {
        {"bg", "assets/section/backgroun.png"},
        {"board", "assets/section/board.png"},
        {"inputSection", "assets/section/inputSection.png"},
        {"titleSection", "assets/section/tittleSection.png"},
        {"iceTile", "assets/tile/iceTile.png"},
        {"lavaTile", "assets/tile/lavaTile.png"},
        {"wallTile", "assets/tile/wallTile.png"},
        {"actor", "assets/add/pinguin.png"},
        {"igloo", "assets/add/Igloo.png"},
        {"radioOn", "assets/button-1/radioButtonSelected.png"},
        {"radioOff", "assets/button-1/radioButtonNotSelected.png"},
        {"btnOk", "assets/button-1/okButton.png"},
        {"btnPlay", "assets/button-1/playButton.png"},
        {"btnPause", "assets/button-1/pauseButton.png"},
        {"btnNext", "assets/button-1/nextButton.png"},
        {"btnPrev", "assets/button-1/prevButton.png"},
        {"btnFirst", "assets/button-1/firstButton.png"},
        {"btnLast", "assets/button-1/lastButton.png"},
        {"btnNextArrow", "assets/button-1/nextArrow.png"},
        {"btnBackArrow", "assets/button-1/backArrow.png"},
        {"btnHome", "assets/button-1/homeButton.png"},
        {"num0", "assets/number/0.png"},
        {"num1", "assets/number/1.png"},
        {"num2", "assets/number/2.png"},
        {"num3", "assets/number/3.png"},
        {"num4", "assets/number/4.png"},
        {"num5", "assets/number/5.png"},
        {"num6", "assets/number/6.png"},
        {"num7", "assets/number/7.png"},
        {"num8", "assets/number/8.png"},
        {"num9", "assets/number/9.png"},
    };
    for (const auto& d : defs) {
        Texture2D tex = LoadTexture(d.path);
        if (tex.id != 0) {
            SetTextureFilter(tex, TEXTURE_FILTER_POINT);
            textures[d.name] = tex;
        }
    }
#endif
}

void AssetManager::unloadAll() {
#ifdef HAVE_RAYLIB
    for (auto& p : textures) UnloadTexture(p.second);
    textures.clear();
#endif
}

#ifdef HAVE_RAYLIB
Texture2D AssetManager::get(const std::string& name) const {
    auto it = textures.find(name);
    if (it != textures.end()) return it->second;
    return Texture2D{0, 0, 0, 0, 0};
}
bool AssetManager::has(const std::string& name) const {
    return textures.count(name) > 0;
}
#endif
