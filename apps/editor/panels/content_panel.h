#pragma once

#include <string>

namespace vulcal::asset_pipeline {
class AssetIndex;
}

namespace editor {

class EditorContext;

class ContentPanel {
public:
    void draw(EditorContext& context, vulcal::asset_pipeline::AssetIndex& assets, bool& open);

private:
    std::string new_material_name_;
    std::string error_;
};

} // namespace editor
