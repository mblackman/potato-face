#pragma once

#include <SDL2/SDL.h>

#include "../AssetManager/AssetManager.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

class RenderSystem : public System {
   public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    ~RenderSystem() = default;

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager) {
        for (auto entity : GetEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            const auto texture = assetManager->GetTexture(sprite.assetId);

            SDL_Rect destRect = {
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)};

            SDL_RenderCopyEx(renderer, texture, &sprite.srcRect, &destRect, transform.rotation, nullptr, SDL_FLIP_NONE);
        }
    }
};