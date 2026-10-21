#include "asset/asset_manager.h"

#include "asset/material_bin.h"
#include "asset/mesh_bin.h"
#include "render/renderer.h"

namespace vulcal::asset {
namespace fs = std::filesystem;

AssetManager::AssetManager(render::Renderer& renderer)
        : renderer_(renderer) {}

void AssetManager::set_cache_directory(fs::path directory) { directory_ = std::move(directory); }

render::MeshHandle AssetManager::mesh(MeshID id) {
    if (id.is_nil() || directory_.empty()) return {};
    const auto key = id.value_.value();
    if (const auto found = meshes_.find(key); found != meshes_.end()) return found->second;

    render::MeshHandle handle;
    if (auto data = decode_mesh_bin(directory_ / (std::to_string(key) + ".vmesh"))) handle = renderer_.create_mesh(*data);
    meshes_.emplace(key, handle);
    return handle;
}

render::MaterialHandle AssetManager::material(MaterialID id) {
    if (id.is_nil() || directory_.empty()) return {};
    const auto key = id.value_.value();
    if (const auto found = materials_.find(key); found != materials_.end()) return found->second;

    render::MaterialHandle handle;
    if (auto material = decode_material_bin(directory_ / (std::to_string(key) + ".vmatc"))) handle = renderer_.create_material(*material);
    materials_.emplace(key, handle);
    return handle;
}

void AssetManager::invalidate(MeshID id) {
    if (id.is_nil()) return;
    const auto key = id.value_.value();
    if (const auto found = meshes_.find(key); found != meshes_.end()) {
        if (found->second.value_ != 0) renderer_.destroy_mesh(found->second);
        meshes_.erase(found);
    }
}

void AssetManager::invalidate(MaterialID id) {
    if (id.is_nil()) return;
    const auto key = id.value_.value();
    if (const auto found = materials_.find(key); found != materials_.end()) {
        if (found->second.value_ != 0) renderer_.destroy_material(found->second);
        materials_.erase(found);
    }
}

void AssetManager::clear() {
    for (const auto& [id, handle]: meshes_)
        if (handle.value_ != 0) renderer_.destroy_mesh(handle);
    meshes_.clear();
    for (const auto& [id, handle]: materials_)
        if (handle.value_ != 0) renderer_.destroy_material(handle);
    materials_.clear();
}

} // namespace vulcal::asset
