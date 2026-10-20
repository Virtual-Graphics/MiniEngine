#include "asset/mesh_cache.h"
#include "asset/obj_loader.h"
#include "render/renderer.h"
#include <algorithm>
#include <cctype>

namespace vulcal::asset {
namespace fs = std::filesystem;
MeshCache::~MeshCache() { clear(); }

render::MeshHandle MeshCache::load(const fs::path& path) {
    const auto key = fs::absolute(path).lexically_normal();
    // Cache failures too: no per-frame loading or automatic retry before clear().
    const auto [entry, inserted] = meshes_.try_emplace(key);
    if (!inserted) return entry->second;
    try {
        auto extension = key.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (extension == ".obj") {
            const auto data = OBJImporter::load_obj(key);
            if (!data.vertices_.empty() && !data.indices_.empty()) {
                entry->second = renderer_.create_mesh(data);
                return entry->second;
            }
        }
        const auto utf8 = key.generic_u8string();
        base::log::warn("MeshCache: could not load mesh {}", std::string(utf8.begin(), utf8.end()));
    } catch (const std::exception& error) {
        base::log::warn("MeshCache: {}", error.what());
    }
    return {};
}

void MeshCache::clear() {
    for (const auto& [path, handle] : meshes_) if (handle.value_) renderer_.destroy_mesh(handle);
    meshes_.clear();
}
} // namespace vulcal::asset
