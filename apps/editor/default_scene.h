#pragma once

#include <memory>

namespace vulcal::scene {
class Scene;
}

namespace editor {

std::unique_ptr<vulcal::scene::Scene> create_default_scene();

} // namespace editor
