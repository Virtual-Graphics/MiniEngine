#pragma once

#include <memory>

namespace mini::render {
class Renderer;
}
namespace mini::scene {
class Scene;
}

namespace editor {

std::unique_ptr<mini::scene::Scene> create_default_scene(mini::render::Renderer& renderer);

} // namespace editor
