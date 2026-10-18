#include "editor/panels/viewport_panel.h"

#include <algorithm>
#include <cmath>
#include <imgui.h>

namespace editor {

void ViewportPanel::draw(bool& open, float camera_speed) {
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
            GLint max_size = 0;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
            const auto pixels = [max_size](float value) {
                return static_cast<std::uint32_t>(std::clamp(std::round(value), 1.0f, static_cast<float>(max_size)));
            };
            const auto width = pixels(size.x * scale.x);
            const auto height = pixels(size.y * scale.y);
            if (!target_) {
                target_ = std::make_unique<mini::gfx::RenderTarget>(mini::gfx::RenderTarget::Spec{ .width = width, .height = height });
            } else {
                target_->resize(width, height);
            }
            // resize() recreates attachments, so obtain the ID only after resizing.
            const ImTextureRef texture{ static_cast<ImTextureID>(target_->color()->image().id()) };
            ImGui::Image(texture, size, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });
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
