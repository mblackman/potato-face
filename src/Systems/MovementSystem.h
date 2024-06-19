#pragma once

#include "../ECS/ECS.h"
#include "../General/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem: public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        ~MovementSystem() = default;
        
        void Update(double deltaTime) {
            for (auto entity: GetEntities()) {
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidBody.velocity.x * deltaTime;
                transform.position.y += rigidBody.velocity.y * deltaTime;
            }
        }
};