#pragma once
#include "render/pipeline/pass.h"
#include <memory>
#include <vector>

namespace vulcal::gfx {
class RenderTarget;
} // namespace vulcal::gfx

namespace vulcal::render {

struct RenderData;

// Owns the ordered passes. Passes run in creation order, so adding a pass is
// just one more make_unique + push_back in the constructor.
class Pipeline {
public:
    Pipeline();

    void render(const RenderData& data, gfx::RenderTarget* target, const std::vector<DrawItem>& items);

private:
    std::vector<std::unique_ptr<Pass>> passes_;
};

} // namespace vulcal::render
