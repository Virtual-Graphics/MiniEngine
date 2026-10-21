#include "editor/editor_context.h"

#include "base/logger.h"

#include <stdexcept>
#include <utility>

namespace editor {

void EditorContext::set_scene(std::unique_ptr<vulcal::scene::Scene> scene) {
    if (!scene) throw std::invalid_argument("EditorContext requires a scene");
    selected_ = {};
    scene_ = std::move(scene);
}

vulcal::scene::Entity EditorContext::selected_entity() const { return scene_->contains(selected_) ? selected_ : vulcal::scene::Entity{}; }

void EditorContext::select(vulcal::scene::Entity entity) {
    if (entity && !scene_->contains(entity)) throw std::invalid_argument("Entity belongs to a different scene");
    selected_ = entity ? entity : vulcal::scene::Entity{};
}

vulcal::scene::Entity EditorContext::create_entity() {
    auto entity = scene_->create_entity();
    select(entity);
    return entity;
}

void EditorContext::destroy_entity(vulcal::scene::Entity entity) {
    if (!entity) return;
    if (!scene_->contains(entity)) {
        base::log::error("Cannot destroy an entity that belongs to a different scene");
        throw std::invalid_argument("Entity belongs to a different scene");
    }
    if (selected_ == entity) selected_ = {};
    scene_->destroy_entity(entity);
}

} // namespace editor
