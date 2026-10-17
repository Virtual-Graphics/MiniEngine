#pragma once

#include <utility>

namespace mini::scene {

inline Entity::Entity(entt::entity handle, Scene& scene)
        : handle_(handle),
          scene_(&scene) {}

inline bool Entity::valid() const { return scene_ != nullptr && scene_->registry().valid(handle_); }

template<typename T>
bool Entity::has_component() const {
    return valid() && scene_->registry().all_of<T>(handle_);
}

template<typename T>
T& Entity::get_component() const {
    return scene_->registry().get<T>(handle_);
}

template<typename T, typename... Args>
T& Entity::add_component(Args&&... args) const {
    return scene_->registry().emplace<T>(handle_, std::forward<Args>(args)...);
}

template<typename T>
void Entity::remove_component() const {
    scene_->registry().remove<T>(handle_);
}

} // namespace mini::scene
