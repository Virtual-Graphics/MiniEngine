#pragma once

#include "scene/scene.h"

#include <memory>

namespace editor {

// Shared document and selection state. Panels never own references to an old scene.
class EditorContext {
public:
    vulcal::scene::Scene& scene() { return *scene_; }
    const vulcal::scene::Scene& scene() const { return *scene_; }
    void set_scene(std::unique_ptr<vulcal::scene::Scene> scene);

    vulcal::scene::Entity selected_entity() const;
    void select(vulcal::scene::Entity entity);
    vulcal::scene::Entity create_entity();
    void destroy_entity(vulcal::scene::Entity entity);

private:
    std::unique_ptr<vulcal::scene::Scene> scene_ = std::make_unique<vulcal::scene::Scene>();
    vulcal::scene::Entity selected_;
};

} // namespace editor
