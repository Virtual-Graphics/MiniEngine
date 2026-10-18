#pragma once

#include "render/camera.h"

namespace editor {

// Camera math is independent of window input and the scene's camera entities.
class EditorCamera {
public:
    struct Input {
        glm::vec3 movement{ 0.0f }; // right, world up, forward
        glm::vec2 look_delta{ 0.0f };
        float wheel = 0.0f;
    };

    void update(const Input& input, float seconds);
    mini::render::CameraData data(float aspect) const;
    float speed() const { return speed_; }

private:
    glm::vec3 forward() const;

    glm::vec3 position_{ 0.0f, 4.0f, 8.0f };
    float yaw_ = -90.0f;
    float pitch_ = -29.357754f;
    float speed_ = 5.0f;
};

} // namespace editor
