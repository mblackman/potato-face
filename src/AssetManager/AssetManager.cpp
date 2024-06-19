#include "AssetManager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../General/Logger.h"

AssetManager::AssetManager() {

}

AssetManager::~AssetManager() {
    ClearAssets();
}

void AssetManager::ClearAssets() {
    for (auto texture: textures_) {
        SDL_DestroyTexture(texture.second);
    }
    textures_.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures_.emplace(assetId, texture);

    Logger::Info("Added texture: " + assetId + " from path: " + path);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetId) const {
    return textures_.at(assetId);
}