#include "ECS.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include "../General/Logger.h"

int IComponent::next_id_ = 0;

// Entity Implementation
int Entity::GetId() const {
    return id_;
}

void Entity::Blam() {
    registry_->BlamEntity(*this);
}

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
    std::size_t entityId;

    if (free_ids_.empty()) {
        entityId = num_entities_++;

        if (entityId >= entity_component_signatures_.size()) {
            entity_component_signatures_.resize(entityId + 1);
        }
    } else {
        entityId = free_ids_.front();
        free_ids_.pop_front();
    }

    Entity entity(entityId, this);

    entities_to_add_.insert(entity);

    Logger::Info("Created entity: " + std::to_string(entityId));

    return entity;
}

void Registry::BlamEntity(const Entity entity) {
    entities_to_remove_.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    const auto& entityComponentSignature = entity_component_signatures_[entityId];

    for (auto& system : systems_) {
        const auto& systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(const Entity entity) {
    for (auto& system : systems_) {
        system.second->RemoveEntity(entity);
    }
}

void Registry::Update() {
    for (auto entity : entities_to_add_) {
        entities_.push_back(entity);
        AddEntityToSystems(entity);
    }

    entities_to_add_.clear();

    for (auto entity : entities_to_remove_) {
        RemoveEntityFromSystems(entity);
        free_ids_.push_front(entity.GetId());
        entity_component_signatures_[entity.GetId()].reset();

        Logger::Info("Entity destroyed: " + std::to_string(entity.GetId()));
    }

    entities_to_remove_.clear();
}