#pragma once

#include <bitset>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../General/Pool.h"

const unsigned int kMaxComponents = 32;

// This used to track which components are present in an entity and which
// entities a system is interested in.
typedef std::bitset<kMaxComponents> Signature;

struct IComponent {
   protected:
    static int next_id_;
};

template <typename T>
class Component : public IComponent {
    static int GetId() {
        static auto id = next_id_++;
        return id;
    }
};

class Entity {
   private:
    int id_;

   public:
    Entity(int id) : id_(id) {}

    int GetId() const {
        return id_;
    }

    bool operator==(const Entity& other) const {
        return id_ == other.id_;
    }

    bool operator!=(const Entity& other) const {
        return id_ != other.id_;
    }

    bool operator<(const Entity& other) const {
        return id_ < other.id_;
    }

    bool operator>(const Entity& other) const {
        return id_ > other.id_;
    }

    bool operator<=(const Entity& other) const {
        return id_ <= other.id_;
    }

    bool operator>=(const Entity& other) const {
        return id_ >= other.id_;
    }
};

class System {
   private:
    Signature component_signature_;
    std::vector<Entity> entities_;

   public:
    System() = default;
    ~System() = default;

    const Signature& GetComponentSignature() const {
        return component_signature_;
    }

    std::vector<Entity> GetEntities() const {
        return entities_;
    }

    void AddEntity(const Entity entity);
    void RemoveEntity(const Entity entity);

    void Update(double delta_time) const;

    template <typename T>
    void RequireComponent();
};

/**
 * Manages the creation and destruction of entities, systems, and components.
 */
class Registry {
   private:
    std::size_t num_entities_;
    std::vector<Entity> entities_;

    std::set<Entity> entities_to_add_;
    std::set<Entity> entities_to_remove_;

    // Each pool at an index corresponds to a component type.
    // [Pool index = entity id]
    std::vector<IPool*> component_pools_;

    // Component signatures are used to track which components are present in
    // an entity and which entities a system is interested in.
    std::vector<Signature> entity_component_signatures_;

    // A map of the systems that are registered with the registry.
    std::unordered_map<std::type_index, System*> systems_;

    template <typename T>
    void AddComponent(const Entity entity, const T component);

    template <typename T>
    void AddSystem(const T* system);

   public:
    Registry() = default;
    ~Registry() = default;

    // Entity management
    Entity CreateEntity();
    void DestroyEntity(const Entity entity);

    // System management
    template <typename T>
    void AddSystem();

    template <typename T, typename... TArgs>
    void AddSystem(TArgs&&... args);

    template <typename T>
    void RemoveSystem();

    template <typename T>
    bool HasSystem() const;

    template <typename T>
    System* GetSystem() const;

    void AddEntityToSystems(const Entity entity);

    // Component management
    template <typename T>
    void AddComponent(const Entity entity);

    template <typename T, typename... TArgs>
    void AddComponent(const Entity entity, TArgs&&... args);

    template <typename T>
    void RemoveComponent(const Entity entity);

    template <typename T>
    bool HasComponent(const Entity entity) const;

    template <typename T>
    T& GetComponent(const Entity entity) const;

    void Update();
};
