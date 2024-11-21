#pragma once

#include <SDL2/SDL.h>

#include <algorithm>

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

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, SDL_Rect& camera) {
        auto entities = GetEntities();
        std::sort(
            entities.begin(),
            entities.end(),
            [](Entity a, Entity b) {
                const auto spriteA = a.GetComponent<SpriteComponent>();
                const auto spriteB = b.GetComponent<SpriteComponent>();
                return spriteA.zIndex < spriteB.zIndex;
            });

        for (auto entity : entities) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            const auto texture = assetManager->GetTexture(sprite.assetId);

            SDL_Rect destRect = {
                static_cast<int>(transform.position.x - camera.x),
                static_cast<int>(transform.position.y - camera.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)};

            SDL_RenderCopyEx(renderer, texture, &sprite.srcRect, &destRect, transform.rotation, nullptr, SDL_FLIP_NONE);
        }
    }
};