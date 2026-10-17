#include "scene/scene.h"

namespace mini::scene {
namespace {

template<typename T>
void copy_component(entt::registry& destination, const entt::registry& source, entt::entity entity) {
    if (const auto* component = source.try_get<T>(entity)) {
        destination.emplace<T>(entity, *component);
    }
}

} // namespace

Scene::Scene(const Scene& other) {
    const auto& entities = *other.registry_.storage<entt::entity>();

    for (auto [entity]: entities.each()) {
        const auto copy = registry_.create(entity);
        copy_component<IDComponent>(registry_, other.registry_, copy);
        copy_component<TagComponent>(registry_, other.registry_, copy);
        copy_component<TransformComponent>(registry_, other.registry_, copy);
        copy_component<MeshRendererComponent>(registry_, other.registry_, copy);
        copy_component<CameraComponent>(registry_, other.registry_, copy);
        copy_component<EnvironmentComponent>(registry_, other.registry_, copy);
    }
}

Scene& Scene::operator=(const Scene& other) {
    if (this != &other) {
        Scene copy(other);
        registry_.swap(copy.registry_);
    }

    return *this;
}

Entity Scene::create_entity(const std::string& name) {
    const auto entity = registry_.create();
    registry_.emplace<IDComponent>(entity);
    registry_.emplace<TagComponent>(entity, TagComponent{ name });
    registry_.emplace<TransformComponent>(entity);
    return Entity{ entity, *this };
}

void Scene::destroy_entity(Entity entity) { registry_.destroy(entity.handle()); }

} // namespace mini::scene
