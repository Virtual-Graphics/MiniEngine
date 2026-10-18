#pragma once

#include "scene/components.h"
#include "scene/entity.h"

#include <entt/entity/registry.hpp>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
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
    bool contains(Entity entity) const;

    // Component/entity removal must be deferred until traversal has finished.
    template<typename... Components, typename Function>
    void each(Function&& function) {
        registry_.view<Components...>().each(
                [&](auto handle, auto&... components) { std::invoke(function, Entity{ handle, *this }, components...); });
    }

    template<typename T, typename... Args>
    T& add_component(Entity entity, Args&&... args) {
        require_entity(entity);
        if (registry_.all_of<T>(entity.handle())) throw std::logic_error("Entity already has this component");
        return registry_.emplace<T>(entity.handle(), std::forward<Args>(args)...);
    }

    template<typename T>
    T& get_component(Entity entity) {
        require_entity(entity);
        auto* component = registry_.try_get<T>(entity.handle());
        if (!component) throw std::out_of_range("Entity does not have this component");
        return *component;
    }

    template<typename T>
    const T& get_component(Entity entity) const {
        require_entity(entity);
        const auto* component = registry_.try_get<T>(entity.handle());
        if (!component) throw std::out_of_range("Entity does not have this component");
        return *component;
    }

    template<typename T>
    bool has_component(Entity entity) const {
        return contains(entity) && registry_.all_of<T>(entity.handle());
    }

    template<typename T>
    void remove_component(Entity entity) {
        require_entity(entity);
        if constexpr (std::is_same_v<T, IDComponent> || std::is_same_v<T, TagComponent> || std::is_same_v<T, TransformComponent>) {
            throw std::logic_error("Required entity components cannot be removed");
        }
        registry_.remove<T>(entity.handle());
    }

    entt::registry& registry() { return registry_; }
    const entt::registry& registry() const { return registry_; }

private:
    void require_entity(Entity entity) const;
    entt::registry registry_;
};

} // namespace mini::scene

#include "scene/entity.inl"
