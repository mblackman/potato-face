#pragma once

#include <SDL2/SDL.h>

#include "../AssetManager/AssetManager.h"
#include "../Components/TextLabelComponent.h"
#include "../ECS/ECS.h"

class RenderTextSystem : public System {
   public:
    RenderTextSystem() {
        RequireComponent<TextLabelComponent>();
    }

    ~RenderTextSystem() = default;

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, SDL_Rect& camera) {
        auto entities = GetEntities();

        for (auto entity : entities) {
            const auto textLabel = entity.GetComponent<TextLabelComponent>();
            const auto font = assetManager->GetFont(textLabel.fontId);
            SDL_Surface* surface = TTF_RenderText_Blended(
                font,
                textLabel.text.c_str(),
                textLabel.color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;

            SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);

            SDL_Rect destRect = {
                static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
                static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
                labelWidth,
                labelHeight};

            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        }
    }
};