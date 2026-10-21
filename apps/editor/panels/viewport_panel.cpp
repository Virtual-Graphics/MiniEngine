#include "editor/panels/viewport_panel.h"
#include "common/imgui_render_target.h"

#include <algorithm>
#include <cmath>
#include <imgui.h>

namespace editor {

void ViewportPanel::draw(bool& open, float camera_speed, const vulcal::gfx::Context::Limits& limits) {
    visible_ = false;
    focused_ = false;
    camera_requested_ = false;
    if (!open) return;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    if (ImGui::Begin("Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::Text("RMB: look | WASD: move | Q/E: down/up | Wheel: speed (%.2f)", camera_speed);
        const auto size = ImGui::GetContentRegionAvail();
        if (size.x >= 1.0f && size.y >= 1.0f) {
            const auto scale = ImGui::GetIO().DisplayFramebufferScale;
            const auto pixels = [](float value, std::uint32_t maximum) {
                return static_cast<std::uint32_t>(std::clamp(std::round(value), 1.0f, static_cast<float>(maximum)));
            };
            const auto width = pixels(size.x * scale.x, limits.max_render_target_width);
            const auto height = pixels(size.y * scale.y, limits.max_render_target_height);
            if (!target_) {
                target_ = std::make_unique<vulcal::gfx::RenderTarget>(vulcal::gfx::RenderTarget::Spec{ .width = width, .height = height });
            } else {
                target_->resize(width, height);
            }
            // resize() recreates attachments, so obtain the ID only after resizing.
            apps::draw_render_target(*target_, size);
            const bool clicked_left = ImGui::IsItemClicked(ImGuiMouseButton_Left);
            const bool clicked_right = ImGui::IsItemClicked(ImGuiMouseButton_Right);
            if (clicked_left || clicked_right) ImGui::SetWindowFocus();
            camera_requested_ = clicked_right;
            visible_ = true;
        }
        focused_ = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace editor
