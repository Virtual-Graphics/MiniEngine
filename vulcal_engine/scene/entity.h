#pragma once

#include <entt/entity/entity.hpp>

namespace vulcal::scene {

class Scene;

class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, Scene& scene);

    entt::entity handle() const { return handle_; }
    Scene& scene() const { return *scene_; }
    bool valid() const;
    explicit operator bool() const { return valid(); }

    template<typename T>
    bool has_component() const;

    template<typename T>
    T& get_component() const;

    template<typename T, typename... Args>
    T& add_component(Args&&... args) const;

    template<typename T>
    void remove_component() const;

    bool operator==(const Entity& other) const { return handle_ == other.handle_ && scene_ == other.scene_; }

private:
    friend class Scene;
    entt::entity handle_ = entt::null;
    Scene* scene_ = nullptr;
};

} // namespace vulcal::scene
