#pragma once

struct ImVec2;
namespace vulcal::gfx {
class RenderTarget;
}

namespace apps {

// Draw the current color attachment using this application's ImGui backend convention.
// The target must retain its attachments until ImGuiIntegration::render() completes.
void draw_render_target(const vulcal::gfx::RenderTarget& target, const ImVec2& size);

} // namespace apps
