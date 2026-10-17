#pragma once

#include "scene/components.h"
#include "scene/entity.h"

#include <entt/entity/registry.hpp>
#include <string>
#include <utility>

namespace mini::scene {

class Scene {
public:
    Scene() = default;
    Scene(const Scene& other);
    Scene& operator=(const Scene& other);
    ~Scene() = default;

    Entity create_entity(const std::string& name = "Entity");
    void destroy_entity(Entity entity);

    template<typename T, typename... Args>
    T& add_component(Entity entity, Args&&... args) {
        return registry_.emplace<T>(entity.handle(), std::forward<Args>(args)...);
    }

    template<typename T>
    T& get_component(Entity entity) {
        return registry_.get<T>(entity.handle());
    }

    template<typename T>
    const T& get_component(Entity entity) const {
        return registry_.get<T>(entity.handle());
    }

    template<typename T>
    bool has_component(Entity entity) const {
        return registry_.all_of<T>(entity.handle());
    }

    template<typename T>
    void remove_component(Entity entity) {
        registry_.remove<T>(entity.handle());
    }

    entt::registry& registry() { return registry_; }
    const entt::registry& registry() const { return registry_; }

private:
    entt::registry registry_;
};

} // namespace mini::scene

#include "scene/entity.inl"
