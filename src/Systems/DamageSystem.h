#pragma once

#include "../Components/BoxColliderComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../General/Logger.h"

class DamageSystem : public System {
   public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
        eventBus->SubscribeEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent& event) {
        auto aId = std::to_string(event.entityA.GetId());
        auto bId = std::to_string(event.entityB.GetId());
        Logger::Info("Damage collision received between: " + aId + " and " + bId + ".");
    }

    void Update() {
    }
};