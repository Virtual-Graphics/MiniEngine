#pragma once
#include "render/pipeline/pass.h"
#include <cstdint>
#include <memory>

namespace vulcal::gfx {
class RenderTarget;
} // namespace vulcal::gfx

namespace vulcal::render {

class ShadowPass : public Pass {
public:
    ShadowPass();
    ~ShadowPass() override;

    const char* name() const override { return "ShadowPass"; }
    bool enabled(const RenderData& data) const override;

    void prepare(PassContext& ctx) override;
    void draw(PassContext& ctx, const DrawItem& item) override;
    void finish(PassContext& ctx) override;

private:
    void ensure_target(std::uint32_t map_size);

    std::unique_ptr<gfx::RenderTarget> target_;
    std::uint32_t map_size_ = 0;
};

} // namespace vulcal::render
