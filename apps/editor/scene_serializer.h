#pragma once
#include "scene/scene.h"
#include <filesystem>
#include <memory>

namespace editor {
class SceneSerializer {
public:
    static void save(const vulcal::scene::Scene& scene, const std::filesystem::path& path);
    static std::unique_ptr<vulcal::scene::Scene> load(const std::filesystem::path& path);
};
} // namespace editor
