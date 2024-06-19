#include "ECS.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "../General/Logger.h"

int IComponent::next_id_ = 0;

// Systems implementation

void System::AddEntity(const Entity entity) {
    entities_.push_back(entity);
}

void System::RemoveEntity(const Entity entity) {
    entities_.erase(std::remove_if(
                        entities_.begin(),
                        entities_.end(),
                        [&entity](const Entity& other) {
                            return other == entity;
                        }),
                    entities_.end());
}

// Registry implementation

Entity Registry::CreateEntity() {
    std::size_t entityId = num_entities_++;

    Entity entity(entityId, this);

    entities_to_add_.insert(entity);

    if (entityId >= entity_component_signatures_.size()) {
        entity_component_signatures_.resize(entityId + 1);
    }

    Logger::Info("Created entity: " + std::to_string(entityId));

    return entity;
}

void Registry::DestroyEntity(const Entity entity) {
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    const auto& entityComponentSignature = entity_component_signatures_[entityId];
    
    for (auto& system: systems_) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::Update() {
    for (auto entity : entities_to_add_) {
        entities_.push_back(entity);
        AddEntityToSystems(entity);
    }

    entities_to_add_.clear();
}