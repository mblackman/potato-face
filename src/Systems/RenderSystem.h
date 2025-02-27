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

        for (auto it = entities.begin(); it != entities.end();) {
            auto transform = it->GetComponent<TransformComponent>();
            auto sprite = it->GetComponent<SpriteComponent>();

            bool isOutsideCamera = false;

            if (sprite.isFixed) {
                isOutsideCamera = (transform.position.x + sprite.width * transform.scale.x < 0 ||
                                   transform.position.x > Game::windowWidth ||
                                   transform.position.y + sprite.height * transform.scale.y < 0 ||
                                   transform.position.y > Game::windowHeight);
            } else {
                isOutsideCamera = (transform.position.x + sprite.width * transform.scale.x < camera.x ||
                                   transform.position.x > camera.x + camera.w ||
                                   transform.position.y + sprite.height * transform.scale.y < camera.y ||
                                   transform.position.y > camera.y + camera.h);
            }

            if (isOutsideCamera) {
                it = entities.erase(it);
            } else {
                it++;
            }
        }

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
            float x = sprite.isFixed ? transform.position.x : transform.position.x - camera.x;
            float y = sprite.isFixed ? transform.position.y : transform.position.y - camera.y;

            SDL_Rect destRect = {
                static_cast<int>(x),
                static_cast<int>(y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)};

            SDL_RenderCopyEx(renderer, texture, &sprite.srcRect, &destRect, transform.rotation, nullptr, sprite.flip);
        }
    }
};