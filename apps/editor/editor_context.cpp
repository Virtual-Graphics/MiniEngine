#include "editor/editor_context.h"

#include <stdexcept>
#include <utility>

namespace editor {

void EditorContext::set_scene(std::unique_ptr<mini::scene::Scene> scene) {
    if (!scene) throw std::invalid_argument("EditorContext requires a scene");
    selected_ = entt::null;
    scene_ = std::move(scene);
}

mini::scene::Entity EditorContext::selected_entity() const {
    return scene_->registry().valid(selected_) ? mini::scene::Entity{ selected_, *scene_ } : mini::scene::Entity{};
}

void EditorContext::select(mini::scene::Entity entity) {
    if (entity && &entity.scene() != scene_.get()) throw std::invalid_argument("Entity belongs to a different scene");
    selected_ = entity ? entity.handle() : entt::null;
}

mini::scene::Entity EditorContext::create_entity() {
    auto entity = scene_->create_entity();
    select(entity);
    return entity;
}

void EditorContext::destroy_entity(mini::scene::Entity entity) {
    if (!entity) return;
    if (&entity.scene() != scene_.get()) throw std::invalid_argument("Entity belongs to a different scene");
    if (selected_ == entity.handle()) selected_ = entt::null;
    scene_->destroy_entity(entity);
}

} // namespace editor
