#pragma once

#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../General/Logger.h"

class MovementSystem : public System {
   public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    ~MovementSystem() = default;

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<MovementSystem, CollisionEvent>(this, &MovementSystem::OnCollision);
    }

    void OnCollision(CollisionEvent& event) {
        auto a = event.entityA;
        auto b = event.entityB;
        auto aId = std::to_string(event.entityA.GetId());
        auto bId = std::to_string(event.entityB.GetId());
        Logger::Info("Movement collision received between: " + aId + " and " + bId + ".");

        if (a.InGroup("enemies") && b.InGroup("obstacles")) {
            OnObstacleCollision(a);
        }

        if (b.InGroup("enemies") && a.InGroup("obstacles")) {
            OnObstacleCollision(b);
        }
    }

    void Update(double deltaTime) {
        for (auto entity : GetEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * deltaTime;
            transform.position.y += rigidBody.velocity.y * deltaTime;

            bool isEntityOutsideMap = (transform.position.x > Game::windowWidth ||
                                       transform.position.y > Game::windowHeight);

            if (!isEntityOutsideMap) {
                if (entity.HasComponent<SpriteComponent>()) {
                    auto sprite = entity.GetComponent<SpriteComponent>();
                    isEntityOutsideMap = (transform.position.x + sprite.width * transform.scale.x < 0 ||
                                          transform.position.y + sprite.height * transform.scale.y < 0);
                } else {
                    isEntityOutsideMap = (transform.position.x < 0 ||
                                          transform.position.y < 0);
                }
            }

            if (isEntityOutsideMap && !entity.HasTag("player")) {
                Logger::Info("Entity went outside map " + std::to_string(entity.GetId()));
                entity.Blam();
            }
        }
    }

   private:
    void OnObstacleCollision(Entity enemy) {
        auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();

        rigidBody.velocity = rigidBody.velocity * -1.0f;

        if (enemy.HasComponent<SpriteComponent>()) {
            auto& sprite = enemy.GetComponent<SpriteComponent>();
            sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        }
    }
};