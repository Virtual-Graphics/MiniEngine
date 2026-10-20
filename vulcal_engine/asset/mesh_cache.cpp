#include "asset/mesh_cache.h"
#include "asset/obj_loader.h"
#include "render/renderer.h"
#include <algorithm>
#include <cctype>

namespace vulcal::asset {
namespace fs = std::filesystem;
MeshCache::~MeshCache() { clear(); }

render::MeshHandle MeshCache::insert(const fs::path& path, const MeshData& data) {
    const auto key = fs::weakly_canonical(fs::absolute(path));
    if (const auto it = meshes_.find(key); it != meshes_.end()) return it->second;
    if (data.vertices_.empty() || data.indices_.empty()) return {};
    const auto handle = renderer_.create_mesh(data);
    meshes_.emplace(key, handle);
    failures_.erase(key);
    return handle;
}

render::MeshHandle MeshCache::load(const fs::path& path) {
    fs::path key;
    try {
        key = fs::weakly_canonical(fs::absolute(path));
        if (const auto it = meshes_.find(key); it != meshes_.end()) return it->second;
        std::error_code error;
        const auto stamp = fs::last_write_time(key, error);
        const auto previous = failures_.find(key);
        if (previous != failures_.end() && previous->second == stamp) return {};
        failures_[key] = stamp;
        auto extension = key.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (extension == ".obj") {
            const auto handle = insert(key, OBJImporter::load_obj(key));
            if (handle.value_) return handle;
        }
        const auto utf8 = key.generic_u8string();
        base::log::warn("MeshCache: could not load mesh {}", std::string(utf8.begin(), utf8.end()));
    } catch (const std::exception& error) {
        base::log::warn("MeshCache: {}", error.what());
    }
    return {};
}

void MeshCache::clear() {
    for (const auto& [path, handle] : meshes_) renderer_.destroy_mesh(handle);
    meshes_.clear();
    failures_.clear();
}
} // namespace vulcal::asset
