#pragma once
#include "render/resources/resources_handle.h"
#include <filesystem>
#include <unordered_map>

namespace vulcal {
class MeshData;
namespace render { class Renderer; }
namespace asset {
// Renderer must outlive this cache. All handles in it belong to that renderer.
class MeshCache {
public:
    explicit MeshCache(render::Renderer& renderer) : renderer_(renderer) {}
    ~MeshCache();
    MeshCache(const MeshCache&) = delete;
    MeshCache& operator=(const MeshCache&) = delete;
    render::MeshHandle load(const std::filesystem::path& path);
    render::MeshHandle insert(const std::filesystem::path& path, const MeshData& data);
    void clear();
private:
    render::Renderer& renderer_;
    std::unordered_map<std::filesystem::path, render::MeshHandle> meshes_;
    // Failed files are retried when their timestamp changes, without logging every frame.
    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> failures_;
};
} // namespace asset
} // namespace vulcal
