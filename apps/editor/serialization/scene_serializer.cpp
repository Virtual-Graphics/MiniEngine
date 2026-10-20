#include "editor/serialization/scene_serializer.h"
#include "editor/serialization/yaml_io.h"
#include "scene/scene.h"

#include <cmath>
#include <numbers>
#include <unordered_set>

namespace editor {
namespace {
using namespace vulcal::scene;

template<typename T>
void field(YAML::Node node, const char* key, T& value, bool loading) {
    if (loading) value = node[key].as<T>();
    else node[key] = value;
    if constexpr (std::is_floating_point_v<T>) {
        if (!std::isfinite(value)) throw std::runtime_error(std::string("Non-finite field: ") + key);
    }
}

template<glm::length_t N>
void field(YAML::Node node, const char* key, glm::vec<N, float>& value, bool loading) {
    if (loading) {
        auto data = node[key];
        if (!data.IsSequence() || data.size() != N) throw std::runtime_error(std::string("Invalid vector: ") + key);
        for (glm::length_t i = 0; i < N; ++i) value[i] = data[i].as<float>();
    } else {
        YAML::Node data(YAML::NodeType::Sequence);
        data.SetStyle(YAML::EmitterStyle::Flow);
        for (glm::length_t i = 0; i < N; ++i) data.push_back(value[i]);
        node[key] = data;
    }
    for (glm::length_t i = 0; i < N; ++i)
        if (!std::isfinite(value[i])) throw std::runtime_error(std::string("Non-finite vector: ") + key);
}

// Version 1's component schema is intentionally explicit and local to the editor.
void fields(YAML::Node n, TransformComponent& c, bool load) {
    field(n, "translation", c.translation_, load);
    field(n, "rotation", c.rotation_, load);
    field(n, "scale", c.scale_, load);
}
void fields(YAML::Node n, MeshRendererComponent& c, bool load) {
    if (load) c.mesh_ = path_from_utf8(n["mesh"].as<std::string>());
    else n["mesh"] = path_to_utf8(c.mesh_.lexically_normal());
    validate_relative_path(c.mesh_, true);
    field(n, "visible", c.visible_, load);
    // Material handles are transient; loaded meshes use the editor's default material.
}
void fields(YAML::Node n, CameraComponent& c, bool load) {
    std::string projection = c.projection_type_ == CameraComponent::ProjectionType::Perspective ? "perspective" : "orthographic";
    field(n, "projection", projection, load);
    if (projection == "perspective") c.projection_type_ = CameraComponent::ProjectionType::Perspective;
    else if (projection == "orthographic") c.projection_type_ = CameraComponent::ProjectionType::Orthographic;
    else throw std::runtime_error("Unknown camera projection");
    field(n, "primary", c.primary_, load);
    field(n, "perspective_fov_y", c.perspective_fov_y_, load);
    field(n, "perspective_aspect", c.perspective_aspect_, load);
    field(n, "perspective_near", c.perspective_near_, load);
    field(n, "perspective_far", c.perspective_far_, load);
    field(n, "orthographic_left", c.orthographic_left_, load);
    field(n, "orthographic_right", c.orthographic_right_, load);
    field(n, "orthographic_bottom", c.orthographic_bottom_, load);
    field(n, "orthographic_top", c.orthographic_top_, load);
    field(n, "orthographic_near", c.orthographic_near_, load);
    field(n, "orthographic_far", c.orthographic_far_, load);
    if (c.perspective_fov_y_ <= 0.0f || c.perspective_fov_y_ >= std::numbers::pi_v<float> ||
            c.perspective_aspect_ <= 0.0f || c.perspective_near_ <= 0.0f || c.perspective_far_ <= c.perspective_near_)
        throw std::runtime_error("Invalid perspective camera bounds");
    if (c.orthographic_right_ <= c.orthographic_left_ || c.orthographic_top_ <= c.orthographic_bottom_ ||
            c.orthographic_far_ <= c.orthographic_near_)
        throw std::runtime_error("Invalid orthographic camera bounds");
}
void fields(YAML::Node n, LightComponent& c, bool load) {
    std::string type = "directional";
    field(n, "type", type, load);
    if (type != "directional" || c.type_ != LightType::Directional) throw std::runtime_error("Unknown light type");
    field(n, "color", c.color_, load);
    field(n, "intensity", c.intensity_, load);
    field(n, "shadow_enabled", c.shadow_enabled_, load);
    field(n, "shadow_map_size", c.shadow_map_size_, load);
    field(n, "shadow_center", c.shadow_center_, load);
    field(n, "shadow_extent", c.shadow_extent_, load);
    field(n, "shadow_near", c.shadow_near_, load);
    field(n, "shadow_far", c.shadow_far_, load);
    field(n, "shadow_depth_bias", c.shadow_depth_bias_, load);
    field(n, "shadow_normal_bias", c.shadow_normal_bias_, load);
    switch (c.shadow_map_size_) {
    case 512: case 1024: case 2048: case 4096: break;
    default: throw std::runtime_error("Shadow map size must be 512, 1024, 2048 or 4096");
    }
    if (c.shadow_extent_ <= 0.0f || c.shadow_near_ < 0.0f || c.shadow_far_ <= c.shadow_near_)
        throw std::runtime_error("Invalid shadow projection bounds");
}
void fields(YAML::Node n, EnvironmentComponent& c, bool load) {
    field(n, "ambient_color", c.ambient_color_, load);
    field(n, "clear_color", c.clear_color_, load);
}

template<typename T>
void save_component(YAML::Node node, const char* key, const entt::registry& registry, entt::entity entity) {
    if (const auto* component = registry.try_get<T>(entity)) {
        auto copy = *component;
        fields(node[key], copy, false);
    }
}

template<typename T>
void load_component(const YAML::Node& node, const char* key, Entity entity) {
    if (auto data = node[key]) {
        if (!data.IsMap()) throw std::runtime_error(std::string("Invalid component: ") + key);
        fields(data, entity.add_component<T>(), true);
    }
}
} // namespace

void SceneSerializer::save(const Scene& scene, const std::filesystem::path& path) {
    YAML::Node root;
    root["format_version"] = 1;
    auto entities = root["entities"] = YAML::Node(YAML::NodeType::Sequence);
    std::unordered_set<std::uint64_t> ids;
    const auto& registry = scene.registry();
    for (const auto entity : registry.view<const IDComponent, const TagComponent, const TransformComponent>()) {
        YAML::Node node;
        const auto id = registry.get<IDComponent>(entity).id_.value();
        if (id == 0 || !ids.insert(id).second) throw std::runtime_error("Scene contains nil or duplicate entity UUIDs");
        node["id"] = id;
        node["name"] = registry.get<TagComponent>(entity).name_;
        save_component<TransformComponent>(node, "transform", registry, entity);
        save_component<MeshRendererComponent>(node, "mesh_renderer", registry, entity);
        save_component<CameraComponent>(node, "camera", registry, entity);
        save_component<LightComponent>(node, "light", registry, entity);
        save_component<EnvironmentComponent>(node, "environment", registry, entity);
        entities.push_back(node);
    }
    yaml_io::write(path, root);
}

std::unique_ptr<Scene> SceneSerializer::load(const std::filesystem::path& path) {
    const auto root = yaml_io::read(path);
    const auto entities = root["entities"];
    if (!entities.IsSequence()) throw std::runtime_error("Scene entities must be a sequence");
    auto scene = std::make_unique<Scene>();
    std::unordered_set<std::uint64_t> ids;
    for (const auto& node : entities) {
        const auto id = node["id"].as<std::uint64_t>();
        if (id == 0 || !ids.insert(id).second) throw std::runtime_error("Scene contains nil or duplicate entity UUIDs");
        auto entity = scene->create_entity(node["name"].as<std::string>());
        entity.get_component<IDComponent>().id_ = base::UUID{ id };
        auto transform = node["transform"];
        if (!transform.IsMap()) throw std::runtime_error("Entity transform is missing");
        fields(transform, entity.get_component<TransformComponent>(), true);
        load_component<MeshRendererComponent>(node, "mesh_renderer", entity);
        load_component<CameraComponent>(node, "camera", entity);
        load_component<LightComponent>(node, "light", entity);
        load_component<EnvironmentComponent>(node, "environment", entity);
    }
    return scene;
}
} // namespace editor
