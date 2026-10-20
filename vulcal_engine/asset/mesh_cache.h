#pragma once
#include "render/resources/resources_handle.h"
#include <filesystem>
#include <unordered_map>

namespace vulcal {
namespace render { class Renderer; }
namespace asset {
// Temporary OBJ path cache; Asset Pipeline will replace this loading path.
// Renderer must outlive this cache. All handles in it belong to that renderer.
class MeshCache {
public:
    explicit MeshCache(render::Renderer& renderer) : renderer_(renderer) {}
    ~MeshCache();
    MeshCache(const MeshCache&) = delete;
    MeshCache& operator=(const MeshCache&) = delete;
    render::MeshHandle load(const std::filesystem::path& path);
    void clear();
private:
    render::Renderer& renderer_;
    std::unordered_map<std::filesystem::path, render::MeshHandle> meshes_;
};
} // namespace asset
} // namespace vulcal
