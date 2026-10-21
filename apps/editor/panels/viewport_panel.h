#pragma once

#include "gfx/context.h"
#include "gfx/render_target.h"

#include <memory>

namespace editor {

class ViewportPanel {
public:
    void draw(bool& open, float camera_speed, const vulcal::gfx::Context::Limits& limits);
    bool visible() const { return visible_; }
    bool focused() const { return focused_; }
    bool camera_requested() const { return camera_requested_; }
    vulcal::gfx::RenderTarget& target() { return *target_; }

private:
    std::unique_ptr<vulcal::gfx::RenderTarget> target_;
    bool visible_ = false;
    bool focused_ = false;
    bool camera_requested_ = false;
};

} // namespace editor
