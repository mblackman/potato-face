#pragma once

#include <SDL2/SDL.h>

#include <string>

struct SpriteComponent {
    std::string assetId;
    int width;
    int height;
    int zIndex;
    bool isFixed;
    SDL_Rect srcRect;
    SDL_RendererFlip flip;

    SpriteComponent(
        std::string assetId = "",
        int width = 0,
        int height = 0,
        int zIndex = 0,
        bool isFixed = false,
        int srcRectX = 0,
        int srcRectY = 0) : assetId(assetId), width(width), height(height), zIndex(zIndex), isFixed(isFixed), flip(SDL_FLIP_NONE) {
        this->srcRect = {srcRectX, srcRectY, width, height};
    }
};