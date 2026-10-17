#include "render/renderer.h"

#include "gfx/debug.h"
#include "gfx/render_target.h"
#include "render/pipeline/pipeline.h"
#include "render/resources/mesh_gpu.h"
#include "resources/material.h"
#include <atomic>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace {
std::uint64_t next_resource_id() {
    static std::uint64_t next{ 1 };
    return next++;
}
} // namespace
namespace mini::render {
struct Renderer::Impl {
    std::unordered_map<std::uint64_t, std::unique_ptr<MeshGPU>> meshes_;
    std::unordered_map<std::uint64_t, Material> materials_;
    Pipeline pipeline_;
};

Renderer::Renderer() : impl_(std::make_unique<Impl>()) {}

Renderer::~Renderer() = default;

MeshHandle Renderer::create_mesh(const MeshData& data) {
    auto mesh = std::make_unique<MeshGPU>(data);
    MeshHandle handle{ next_resource_id() };
    impl_->meshes_.emplace(handle.value_, std::move(mesh));
    return handle;
}

void Renderer::destroy_mesh(MeshHandle mesh) {
    if (impl_->meshes_.erase(mesh.value_) == 0) {
        throw std::invalid_argument("Invalid mesh handle");
    }
}

MaterialHandle Renderer::create_material(const Material& material) {
    MaterialHandle handle{ next_resource_id() };
    impl_->materials_.emplace(handle.value_, material);
    return handle;
}

void Renderer::update_material(MaterialHandle handle, const Material& material) {
    auto it = impl_->materials_.find(handle.value_);
    if (it == impl_->materials_.end()) {
        throw std::invalid_argument("Invalid material handle");
    }
    it->second = material;
}

void Renderer::destroy_material(MaterialHandle material) {
    if (impl_->materials_.erase(material.value_) == 0) {
        throw std::invalid_argument("Invalid material handle");
    }
}

void Renderer::render(const RenderData& data, gfx::RenderTarget* target) {

    gfx::DebugScope scope("Renderer::render");

    std::vector<DrawItem> items;
    items.reserve(data.items_.size());
    for (const auto& item: data.items_) {
        items.push_back(DrawItem{ .mesh_ = impl_->meshes_.at(item.mesh_.value_).get(),
            .material_ = &impl_->materials_.at(item.material_.value_),
            .world_transform_ = item.world_transform_ });
    }

    impl_->pipeline_.render(data, target, items);
}

} // namespace mini::render