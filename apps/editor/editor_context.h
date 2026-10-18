#pragma once

#include "scene/scene.h"

#include <memory>

namespace editor {

// Shared document and selection state. Panels never own references to an old scene.
class EditorContext {
public:
    mini::scene::Scene& scene() { return *scene_; }
    const mini::scene::Scene& scene() const { return *scene_; }
    void set_scene(std::unique_ptr<mini::scene::Scene> scene);

    mini::scene::Entity selected_entity() const;
    void select(mini::scene::Entity entity);
    mini::scene::Entity create_entity();
    void destroy_entity(mini::scene::Entity entity);

private:
    std::unique_ptr<mini::scene::Scene> scene_ = std::make_unique<mini::scene::Scene>();
    entt::entity selected_ = entt::null;
};

} // namespace editor
