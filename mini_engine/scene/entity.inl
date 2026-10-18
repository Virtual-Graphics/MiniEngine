#pragma once

#include <utility>

namespace mini::scene {

inline Entity::Entity(entt::entity handle, Scene& scene)
        : handle_(handle),
          scene_(&scene) {}

inline bool Entity::valid() const { return scene_ != nullptr && scene_->contains(*this); }

template<typename T>
bool Entity::has_component() const {
    return scene_ != nullptr && scene_->has_component<T>(*this);
}

template<typename T>
T& Entity::get_component() const {
    if (!scene_) throw std::invalid_argument("Entity has no scene");
    return scene_->get_component<T>(*this);
}

template<typename T, typename... Args>
T& Entity::add_component(Args&&... args) const {
    if (!scene_) throw std::invalid_argument("Entity has no scene");
    return scene_->add_component<T>(*this, std::forward<Args>(args)...);
}

template<typename T>
void Entity::remove_component() const {
    if (!scene_) throw std::invalid_argument("Entity has no scene");
    scene_->remove_component<T>(*this);
}

} // namespace mini::scene
