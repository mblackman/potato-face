#pragma once

#include <sol/sol.hpp>

#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../General/Logger.h"

int GetEntityPosition(Entity entity) {
    if (!entity.HasComponent<TransformComponent>()) {
        Logger::Error("Entity does not have TransformComponent.");
        return 0;
    }

    auto transform = entity.GetComponent<TransformComponent>();
    return transform.position.x;
}

void SetEntityPosition(Entity entity, double x, double y) {
    if (!entity.HasComponent<TransformComponent>()) {
        Logger::Error("Entity does not have TransformComponent.");
        return;
    }

    auto& transform = entity.GetComponent<TransformComponent>();
    transform.position.x = x;
    transform.position.y = y;
}

class ScriptSystem : public System {
   public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    ~ScriptSystem() = default;

    void CreateLuaBindings(sol::state& lua) {
        lua.new_usertype<Entity>(
            "entity",
            "get_id", &Entity::GetId,
            "blam", &Entity::Blam,
            "has_tag", &Entity::HasTag,
            "in_group", &Entity::InGroup);
        lua.set_function("get_position", &GetEntityPosition);
        lua.set_function("set_position", &SetEntityPosition);
    }

    void Update(double deltaTime, int elapsedTime) {
        for (auto entity : GetEntities()) {
            auto& script = entity.GetComponent<ScriptComponent>();
            script.updateFunction(entity, deltaTime, elapsedTime);
        }
    }
};