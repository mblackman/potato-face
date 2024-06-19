#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetManager {
    private:
        std::map<std::string, SDL_Texture*> textures_;

    public:
        AssetManager();
        ~AssetManager();

        void ClearAssets();
        void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& path);
        SDL_Texture* GetTexture(const std::string& assetId) const;
};