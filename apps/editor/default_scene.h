#pragma once

#include <memory>

namespace vulcal::render {
class Renderer;
}
namespace vulcal::scene {
class Scene;
}

namespace vulcal::asset { class MeshCache; }

namespace editor {

std::unique_ptr<vulcal::scene::Scene> create_default_scene(vulcal::render::Renderer& renderer, vulcal::asset::MeshCache& cache);

} // namespace editor
