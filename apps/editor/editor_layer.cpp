#include "editor/editor_layer.h"

#include "base/input.h"
#include "base/logger.h"
#include "common/imgui_integration.h"
#include "editor/default_scene.h"
#include "engine/application.h"
#include "gfx/command.h"
#include "render/renderer.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace editor {

EditorLayer::EditorLayer()
        : engine::Layer("EditorLayer") {}

EditorLayer::~EditorLayer() = default;

void EditorLayer::on_attach() {
    imgui_ = std::make_unique<apps::ImGuiIntegration>(application().get_window());
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "mini_editor.ini";
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    renderer_ = std::make_unique<mini::render::Renderer>();
    context_.set_scene(create_default_scene(*renderer_));
}

void EditorLayer::on_detach() {
    release_camera();
    imgui_.reset();
}

void EditorLayer::draw_workspace() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", nullptr, &hierarchy_open_);
            ImGui::MenuItem("Inspector", nullptr, &inspector_open_);
            ImGui::MenuItem("Viewport", nullptr, &viewport_open_);
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) {
                reset_layout_ = true;
                hierarchy_open_ = inspector_open_ = viewport_open_ = true;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::EndMainMenuBar();
    }

    const auto* viewport = ImGui::GetMainViewport();
    const ImGuiID dockspace = ImGui::GetID("EditorDockSpace");
    if (reset_layout_ || ImGui::DockBuilderGetNode(dockspace) == nullptr) {
        // DockBuilder's internal API is confined to initial layout creation.
        ImGui::DockBuilderRemoveNode(dockspace);
        ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace, viewport->WorkSize);
        ImGuiID center = dockspace;
        const ImGuiID left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.18f, nullptr, &center);
        const ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.28f, nullptr, &center);
        ImGui::DockBuilderDockWindow("Hierarchy", left);
        ImGui::DockBuilderDockWindow("Inspector", right);
        ImGui::DockBuilderDockWindow("Viewport", center);
        ImGui::DockBuilderFinish(dockspace);
        reset_layout_ = false;
    }
    ImGui::DockSpaceOverViewport(dockspace, viewport);
}

void EditorLayer::on_update(base::Timestep dt) {
    imgui_->begin_frame();
    draw_workspace();
    hierarchy_.draw(context_, hierarchy_open_);
    inspector_.draw(context_, inspector_open_);
    viewport_.draw(viewport_open_, camera_.speed(), application().get_graphics_context().limits());
    update_camera(dt);
    imgui_->end_frame();
}

void EditorLayer::update_camera(base::Timestep dt) {
    auto* window = application().get_window().handle();
    const bool focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    const bool popup = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
    const bool escape = base::Input::is_key_down(base::Key::Escape);
    const bool right_down = base::Input::is_mouse_button_down(base::MouseButton::Right);
    if (controlling_camera_ && (!right_down || escape || !focused || !viewport_.visible() || !viewport_.focused() || popup)) {
        release_camera();
    }

    bool started = false;
    if (!controlling_camera_ && viewport_.camera_requested() && right_down && focused && !escape && !popup) {
        const auto [x, y] = base::Input::mouse_position();
        saved_mouse_x_ = x;
        saved_mouse_y_ = y;
        if (SDL_SetWindowRelativeMouseMode(window, true)) {
            controlling_camera_ = true;
            started = true;
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            SDL_HideCursor();
        } else {
            base::log::warn("Could not capture editor camera mouse: {}", SDL_GetError());
        }
    }
    if (!controlling_camera_) return;

    camera_.update_from_input(dt.get_seconds(), started);
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
}

void EditorLayer::release_camera() {
    if (!controlling_camera_) return;
    auto* window = application().get_window().handle();
    SDL_SetWindowRelativeMouseMode(window, false);
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0) {
        SDL_WarpMouseInWindow(window, saved_mouse_x_, saved_mouse_y_);
    }
    SDL_ShowCursor();
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    controlling_camera_ = false;
}

void EditorLayer::on_render() {
    if (viewport_.visible()) {
        auto& target = viewport_.target();
        mini::scene::SceneExtractor::Options options;
        options.camera_override_ = camera_.data(static_cast<float>(target.width()) / static_cast<float>(target.height()));
        renderer_->render(extractor_.extract(context_.scene(), options), &target);
    }
    mini::gfx::RenderTarget::unbind();
    const auto& window = application().get_window();
    mini::gfx::Command::set_viewport(window.width(), window.height());
    mini::gfx::Command::clear(glm::vec4{ 0.08f, 0.08f, 0.08f, 1.0f });
    imgui_->render();
}

} // namespace editor
