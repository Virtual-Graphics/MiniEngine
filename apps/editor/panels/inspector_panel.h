#pragma once

#include "base/uuid.h"
#include "render/resources/material.h"

#include <string>

namespace vulcal::asset_pipeline {
class AssetIndex;
}
namespace vulcal::scene {
class Entity;
}

namespace editor {

class EditorContext;

class InspectorPanel {
public:
    void draw(EditorContext& context, vulcal::asset_pipeline::AssetIndex& assets, bool& open);

private:
    void draw_components(EditorContext& context, vulcal::asset_pipeline::AssetIndex& assets, vulcal::scene::Entity entity);
    void draw_asset(vulcal::asset_pipeline::AssetIndex& assets, base::UUID id);

    base::UUID editing_;
    vulcal::render::Material draft_;
    bool draft_loaded_ = false;
    std::string asset_error_;
};

} // namespace editor
