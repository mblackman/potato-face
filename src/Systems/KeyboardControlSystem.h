#pragma once

#include <glm/glm.hpp>

#include "../Components/KeyboardControlComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyInputEvent.h"
#include "../General/Logger.h"

class KeyboardControlSystem : public System {
   public:
    KeyboardControlSystem() {
        RequireComponent<KeyboardControlComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<SpriteComponent>();
    }

    ~KeyboardControlSystem() = default;

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<KeyboardControlSystem, KeyInputEvent>(this, &KeyboardControlSystem::OnKeyInput);
    }

    void OnKeyInput(KeyInputEvent& event) {
        if (!event.isPressed) {
            // rigidBodyComponent.velocity = glm::vec2(0, 0);
            return;
        }

        for (auto entity : GetEntities()) {
            auto& keyboardComponent = entity.GetComponent<KeyboardControlComponent>();
            auto& spriteComponent = entity.GetComponent<SpriteComponent>();
            auto& rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();

            switch (event.inputKey) {
                case InputKey::Key_Up:
                    rigidBodyComponent.velocity = glm::vec2(0, -keyboardComponent.velocity);
                    spriteComponent.srcRect.y = spriteComponent.height * 0;
                    break;
                case InputKey::Key_Down:
                    rigidBodyComponent.velocity = glm::vec2(0, keyboardComponent.velocity);
                    spriteComponent.srcRect.y = spriteComponent.height * 1;
                    break;
                case InputKey::Key_Left:
                    rigidBodyComponent.velocity = glm::vec2(-keyboardComponent.velocity, 0);
                    spriteComponent.srcRect.y = spriteComponent.height * 2;
                    break;
                case InputKey::Key_Right:
                    rigidBodyComponent.velocity = glm::vec2(keyboardComponent.velocity, 0);
                    spriteComponent.srcRect.y = spriteComponent.height * 3;
                    break;
            }
        }
    }

    void Update() {
    }
};