#pragma once
#include "gfx/uniform_buffer.h"
#include "render/pipeline/pass.h"
#include "render/resources/material.h"

namespace vulcal::gfx {
class RenderTarget;
} // namespace vulcal::gfx

namespace vulcal::render {

class ForwardPass : public Pass {
public:
    ForwardPass();

    const char* name() const override { return "ForwardPass"; }

    void prepare(PassContext& ctx) override;
    void draw(PassContext& ctx, const DrawItem& item) override;
    void finish(PassContext& ctx) override;

private:
    void set_light(const RenderData& data);

    gfx::UniformBuffer camera_ubo_;
};

} // namespace vulcal::render
