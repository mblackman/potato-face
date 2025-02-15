#pragma once

#include <SDL2/SDL.h>

#include "../Components/SquarePrimitiveComponent.h"
#include "../ECS/ECS.h"

class RenderPrimitiveSystem : public System {
   public:
    RenderPrimitiveSystem() {
    }

    ~RenderPrimitiveSystem() = default;

    void Update(SDL_Renderer* renderer, SDL_Rect& camera) {
        auto entities = GetEntities();

        for (auto entity : entities) {
            if (entity.HasComponent<SquarePrimitiveComponent>()) {
                RenderSquare(renderer, camera, entity.GetComponent<SquarePrimitiveComponent>());
            }
        }
    }

    void RenderSquare(SDL_Renderer* renderer, SDL_Rect& camera, SquarePrimitiveComponent& square) {
        float x = square.isFixed ? square.position.x : square.position.x - camera.x;
        float y = square.isFixed ? square.position.y : square.position.y - camera.y;

        SDL_Rect rect = {
            static_cast<int>(x),
            static_cast<int>(y),
            square.width,
            square.height};

        SDL_SetRenderDrawColor(renderer, square.color.r, square.color.g, square.color.b, square.color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
};