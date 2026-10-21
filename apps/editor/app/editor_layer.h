#pragma once

#include "common/asset/asset_index.h"
#include "editor/app/editor_context.h"
#include "editor/dialogs/dialog.h"
#include "editor/dialogs/new_project_dialog.h"
#include "editor/panels/content_panel.h"
#include "editor/panels/hierarchy_panel.h"
#include "editor/panels/inspector_panel.h"
#include "editor/panels/viewport_panel.h"
#include "editor/project/project_session.h"
#include "editor/scene/editor_camera.h"
#include "engine/layer.h"
#include "scene/scene_extractor.h"

#include <memory>

namespace apps {
class ImGuiIntegration;
}
namespace vulcal::render {
class Renderer;
}
namespace vulcal::asset {
class AssetManager;
}

namespace editor {

class EditorLayer : public engine::Layer {
public:
    EditorLayer();
    ~EditorLayer() override;

    void on_attach() override;
    void on_detach() override;
    void on_update(base::Timestep dt) override;
    void on_render() override;

private:
    void draw_workspace();
    void update_camera(base::Timestep dt);
    void release_camera();
    void draw_file_menu();
    void save_scene_as();
    void project_changed();
    void run_operation(const std::function<void()>& operation);
    std::filesystem::path scene_directory() const;

    std::unique_ptr<apps::ImGuiIntegration> imgui_;
    std::unique_ptr<vulcal::render::Renderer> renderer_;
    std::unique_ptr<vulcal::asset::AssetManager> asset_manager_;
    std::unique_ptr<class NativeFileDialog> file_dialogs_;
    vulcal::render::MaterialHandle default_material_;
    EditorContext context_;
    vulcal::asset_pipeline::AssetIndex assets_;
    ProjectSession session_{ context_ };
    NewProjectDialog new_project_;
    MessageDialog error_dialog_;
    EditorCamera camera_;
    vulcal::scene::SceneExtractor extractor_;
    HierarchyPanel hierarchy_;
    ContentPanel content_;
    InspectorPanel inspector_;
    ViewportPanel viewport_;
    std::uint64_t assets_revision_ = 0;
    bool hierarchy_open_ = true;
    bool content_open_ = true;
    bool inspector_open_ = true;
    bool viewport_open_ = true;
    bool reset_layout_ = false;
    bool controlling_camera_ = false;
    float saved_mouse_x_ = 0.0f;
    float saved_mouse_y_ = 0.0f;
};

} // namespace editor
