#pragma once

#ifdef HAVE_RAYLIB
#include <raylib.h>
#endif
#include <string>
#include <unordered_map>

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    void loadAll();
    void unloadAll();

#ifdef HAVE_RAYLIB
    Texture2D get(const std::string& name) const;
    bool has(const std::string& name) const;
#endif

private:
#ifdef HAVE_RAYLIB
    std::unordered_map<std::string, Texture2D> textures;
#endif
};
