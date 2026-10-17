#pragma once

#include "render/render_data.h"
#include "scene/components.h"

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

private:
    static render::CameraData make_camera_data(const CameraComponent& camera, const glm::mat4& world_transform) {
        if (camera.projection_type_ == CameraComponent::ProjectionType::Perspective) {
            const render::PerspectiveCamera::Spec spec{
                .fov_y_ = camera.perspective_fov_y_,
                .aspect_ = camera.perspective_aspect_,
                .near_ = camera.perspective_near_,
                .far_ = camera.perspective_far_,
            };
            return render::PerspectiveCamera{ spec }.data(world_transform);
        }

        const render::OrthographicCamera::Spec spec{
            .left_ = camera.orthographic_left_,
            .right_ = camera.orthographic_right_,
            .bottom_ = camera.orthographic_bottom_,
            .top_ = camera.orthographic_top_,
            .near_ = camera.orthographic_near_,
            .far_ = camera.orthographic_far_,
        };
        return render::OrthographicCamera{ spec }.data(world_transform);
    }
};

} // namespace mini::scene
