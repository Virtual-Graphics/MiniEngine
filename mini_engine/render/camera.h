#pragma once
#include <glm/glm.hpp>

namespace mini::render {

struct CameraData {
    glm::mat4 view_{ 1.0f };
    glm::mat4 projection_{ 1.0f };
    glm::vec3 position_{ 0.0f };
};

class Camera {
public:
    Camera();
    virtual ~Camera();

    CameraData data(const glm::mat4& world_transform) const;

    virtual glm::mat4 projection() const = 0;
};

class PerspectiveCamera : public Camera {
public:
    struct Spec {
        float fov_y_ = glm::radians(45.0f);
        float aspect_ = 1.0f;
        float near_ = 0.1f;
        float far_ = 100.0f;
    };

    PerspectiveCamera();
    PerspectiveCamera(const Spec& spec);

    void set_spec(const Spec& spec);

    glm::mat4 projection() const override;

private:
    Spec spec_;
};

class OrthographicCamera : public Camera {
public:
    struct Spec {
        float left_ = -1.0f;
        float right_ = 1.0f;
        float bottom_ = -1.0f;
        float top_ = 1.0f;
        float near_ = 0.1f;
        float far_ = 100.0f;
    };

    OrthographicCamera();
    OrthographicCamera(const Spec& spec);

    void set_spec(const Spec& spec);

    glm::mat4 projection() const override;

private:
    Spec spec_;
};

} // namespace mini::render
