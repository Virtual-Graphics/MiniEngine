#pragma once

#include "render/render_data.h"

#include <optional>

namespace mini::scene {

class Scene;

class SceneExtractor {
public:
    enum class CameraPolicy { Primary, First };

    struct Options {
        CameraPolicy camera_policy_ = CameraPolicy::Primary;
        std::optional<render::CameraData> camera_override_;
    };

    render::RenderData extract(const Scene& scene) const;
    render::RenderData extract(const Scene& scene, const Options& options) const;
};

} // namespace mini::scene
