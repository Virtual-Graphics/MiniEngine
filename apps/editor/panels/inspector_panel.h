#pragma once
#include "scene/entity.h"
#include <functional>

namespace editor {

class EditorContext;

class InspectorPanel {
public:
    void draw(EditorContext& context, bool& open) const;
    std::function<void(vulcal::scene::Entity)> pick_mesh;
};

} // namespace editor
