#include "common/imgui/imgui_render_target.h"

#include "gfx/render_target.h"

#include <imgui.h>
#include <stdexcept>

namespace apps {

void draw_render_target(const vulcal::gfx::RenderTarget& target, const ImVec2& size) {
    if (!target.color()) throw std::invalid_argument("ImGui image requires a render target color attachment");
    // OpenGL3's ImGui backend consumes texture names, with vertically flipped framebuffer UVs.
    const ImTextureRef texture{ static_cast<ImTextureID>(target.color()->image().id()) };
    ImGui::Image(texture, size, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });
}

} // namespace apps
