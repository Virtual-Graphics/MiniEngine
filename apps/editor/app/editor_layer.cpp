#include "editor/app/editor_layer.h"

#include "base/input.h"
#include "base/logger.h"
#include "common/imgui_integration.h"
#include "editor/dialogs/native_file_dialog.h"
#include "asset/mesh_cache.h"
#include "engine/application.h"
#include "gfx/command.h"
#include "render/renderer.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace editor {

EditorLayer::EditorLayer()
        : engine::Layer("EditorLayer"), new_project_([this](const auto& directory, const auto& name, const auto& author, const auto& assets) {
            session_.create_project(directory, name, author, assets);
            project_changed();
        }) {}

EditorLayer::~EditorLayer() = default;

void EditorLayer::on_attach() {
    imgui_ = std::make_unique<apps::ImGuiIntegration>(application().get_window());
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "vulcal_editor.ini";
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    renderer_ = std::make_unique<vulcal::render::Renderer>();
    mesh_cache_ = std::make_unique<vulcal::asset::MeshCache>(*renderer_);
    default_material_ = renderer_->create_material(vulcal::render::Material{});
    file_dialogs_ = std::make_unique<NativeFileDialog>();
    base::log::info("Editor initialized without a project");
}

void EditorLayer::on_detach() {
    release_camera();
    mesh_cache_.reset();
    renderer_.reset();
    file_dialogs_.reset();
    imgui_.reset();
}

void EditorLayer::run_operation(const std::function<void()>& operation) {
    release_camera();
    try { operation(); }
    catch (const std::exception& error) {
        base::log::error("Editor operation failed: {}", error.what());
        error_dialog_.show(error.what());
    }
}

void EditorLayer::project_changed() {
    mesh_cache_->clear();
    camera_ = EditorCamera{};
    base::log::info("Opened project: {}", context_.project()->name_);
}

std::filesystem::path EditorLayer::scene_directory() const {
    const auto& project = *context_.project();
    return project.resolve(project.asset_directory_) / "scenes";
}

void EditorLayer::save_scene_as() {
    const auto current = context_.scene_path();
    const auto directory = current.empty() ? scene_directory() : current.parent_path();
    const auto name = current.empty() ? std::filesystem::path{ "Untitled.vscene" } : current.filename();
    // NFD needs an existing initial folder. Remove a newly created empty folder
    // on cancel (or when the user saves elsewhere).
    struct EmptyDirectory {
        std::filesystem::path path;
        ~EmptyDirectory() {
            if (!path.empty()) { std::error_code ignored; std::filesystem::remove(path, ignored); }
        }
    } cleanup{ std::filesystem::exists(directory) ? std::filesystem::path{} : directory };
    std::filesystem::create_directories(directory);
    if (auto path = NativeFileDialog::save_scene(directory, name)) session_.save_scene(*path);
}

void EditorLayer::draw_file_menu() {
    if (!ImGui::BeginMenu("File")) return;
    if (ImGui::MenuItem("New Project...")) { release_camera(); new_project_.open(); }
    if (ImGui::MenuItem("Open Project...")) run_operation([&] {
        if (auto path = NativeFileDialog::open("Vulcal Project", "vproj")) {
            session_.open_project(*path);
            project_changed();
        }
    });
    ImGui::Separator();
    ImGui::BeginDisabled(!context_.project());
    if (ImGui::MenuItem("New Scene")) run_operation([&] { session_.new_scene(); });
    if (ImGui::MenuItem("Open Scene...")) run_operation([&] {
        auto directory = scene_directory();
        if (!std::filesystem::exists(directory)) directory = directory.parent_path();
        if (auto path = NativeFileDialog::open("Vulcal Scene", "vscene", directory)) session_.open_scene(*path);
    });
    if (ImGui::MenuItem("Save Scene")) run_operation([&] {
        if (context_.scene_path().empty()) save_scene_as();
        else session_.save_scene();
    });
    if (ImGui::MenuItem("Save Scene As...")) run_operation([&] { save_scene_as(); });
    ImGui::EndDisabled();
    ImGui::EndMenu();
}

void EditorLayer::draw_workspace() {
    if (ImGui::BeginMainMenuBar()) {
        draw_file_menu();
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
        if (context_.project()) {
            ImGui::TextUnformatted(context_.project()->name_.c_str());
            ImGui::TextUnformatted(context_.scene_path().empty() ? " / Untitled scene" : path_to_utf8(context_.scene_path().filename()).c_str());
            ImGui::Separator();
        }
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
    viewport_.draw(viewport_open_, camera_.speed(), application().get_graphics_context().limits(), context_.has_scene());
    new_project_.draw();
    error_dialog_.draw();
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
            base::log::debug("Editor camera control started");
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
    base::log::debug("Editor camera control released");
}

void EditorLayer::on_render() {
    if (context_.has_scene() && viewport_.visible()) {
        auto& target = viewport_.target();
        vulcal::scene::SceneExtractor::Options options;
        options.camera_override_ = camera_.data(static_cast<float>(target.width()) / static_cast<float>(target.height()));
        options.default_material_ = default_material_;
        options.mesh_cache_ = mesh_cache_.get();
        options.mesh_base_directory_ = context_.project()->root_directory();
        renderer_->render(extractor_.extract(context_.scene(), options), &target);
    }
    vulcal::gfx::RenderTarget::unbind();
    const auto& window = application().get_window();
    vulcal::gfx::Command::set_viewport(window.width(), window.height());
    vulcal::gfx::Command::clear(glm::vec4{ 0.08f, 0.08f, 0.08f, 1.0f });
    imgui_->render();
}

} // namespace editor
