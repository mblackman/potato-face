#include "ECS.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>

#include "../General/Logger.h"

int IComponent::next_id_ = 0;

// Systems implementation
template <typename T>
void System::RequireComponent() {
    const auto componentId = Component<T>::GetId();
    component_signature_.set(componentId);
}

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

void System::Update(double deltaTime) const {
    
}

// Registry implementation

Entity Registry::CreateEntity() {
    std::size_t entityId = num_entities_++;

    Entity entity(entityId);
    entities_to_add_.insert(entity);

    if (entityId >= entity_component_signatures_.size()) {
        entity_component_signatures_.resize(entityId + 1);
    }

    Logger::Info("Created entity: " + std::to_string(entityId));

    return entity;
}

void Registry::DestroyEntity(const Entity entity) {
}

template <typename T>
void Registry::AddSystem() {
    T* newSystem();
    AddSystem<T>(newSystem);
}

template <typename T, typename... TArgs>
void Registry::AddSystem(TArgs&&... args) {
    T* newSystem(std::forward<TArgs>(args)...);
    AddSystem<T>(newSystem);
}

template <typename T>
void Registry::AddSystem(const T* system) {
    systems_.insert(std::make_pair(std::type_index(typeid(T)), system));
}

template <typename T>
void Registry::RemoveSystem() {
    auto it = systems_.find(std::type_index(typeid(T)));

    if (it != systems_.end()) {
        systems_.erase(it);
    }
}

template <typename T>
bool Registry::HasSystem() const {
    return systems_.find(std::type_index(typeid(T))) != systems_.end();
}

template <typename T>
System* Registry::GetSystem() const {
    auto it = systems_.find(std::type_index(typeid(T)));

    if (it != systems_.end()) {
        return *(std::static_pointer_cast<T>(it->second));
    }

    return nullptr;
}

void Registry::AddEntityToSystems(const Entity entity) {
    const auto entityId = entity.GetId();

    const auto& entityComponentSignature = entity_component_signatures_[entityId];

    for (const auto& system : systems_) {
        const auto systemComponentSignature = system.second->GetComponentSignature();

        if ((entityComponentSignature & systemComponentSignature) == systemComponentSignature) {
            system.second->AddEntity(entity);
        }
    }
}

void Registry::Update() {
    for (auto entity: entities_to_add_) {
        AddEntityToSystems(entity);
    }

    entities_to_add_.clear();
}

template <typename T>
void Registry::AddComponent(const Entity entity) {
    T newComponent;
    AddComponent<T>(entity, newComponent);
}

template <typename T, typename... TArgs>
void Registry::AddComponent(const Entity entity, TArgs&&... args) {
    T newComponent(std::forward<TArgs>(args)...);
    AddComponent<T>(entity, newComponent);
}

template <typename T>
void Registry::AddComponent(const Entity entity, const T component) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= component_pools_.size()) {
        component_pools_.resize(componentId + 1, nullptr);
    }

    if (!component_pools_[componentId]) {
        Pool<T> newComponentPool = new Pool<T>();
        component_pools_[componentId] = newComponentPool;
    }

    auto componentPool = component_pools_[componentId];

    if (entityId >= componentPool->GetSize()) {
        componentPool.resize(num_entities_);
    }

    componentPool->Set(entityId, component);

    entity_component_signatures_[entityId].set(componentId);

    Logger::Info("Added component: " + std::to_string(entityId) + " to entity: " + std::to_string(entityId));
}

template <typename T>
void Registry::RemoveComponent(const Entity entity) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    entity_component_signatures_[entityId].set(componentId, false);

    Logger::Info("Removed component: " + std::to_string(entityId) + " from entity: " + std::to_string(entityId));
}

template <typename T>
bool Registry::HasComponent(const Entity entity) const {
    return entity_component_signatures_[entity.GetId()].test(Component<T>::GetId());
}

template <typename T>
T& Registry::GetComponent(const Entity entity) const {
}
