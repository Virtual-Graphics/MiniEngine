#pragma once

#include "asset/asset_id.h"
#include "render/resources/resources_handle.h"

#include <cstdint>
#include <filesystem>
#include <unordered_map>

namespace vulcal::render {
class Renderer;
}

namespace vulcal::asset {
// Resolves persistent asset identities to session-scoped Renderer handles and caches
// both successes and failures. Failed lookups are retried only after invalidate()/clear().
// The Renderer must outlive the manager.
class AssetManager {
public:
    explicit AssetManager(render::Renderer& renderer);

    void set_cache_directory(std::filesystem::path directory);
    const std::filesystem::path& cache_directory() const { return directory_; }

    render::MeshHandle mesh(MeshID id);
    render::MaterialHandle material(MaterialID id);
    void invalidate(MeshID id);
    void invalidate(MaterialID id);
    void clear();

private:
    render::Renderer& renderer_;
    std::filesystem::path directory_;
    std::unordered_map<std::uint64_t, render::MeshHandle> meshes_;
    std::unordered_map<std::uint64_t, render::MaterialHandle> materials_;
};

} // namespace vulcal::asset
