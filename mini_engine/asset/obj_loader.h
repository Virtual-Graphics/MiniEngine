#pragma once
#include <filesystem>
#include <string>
#include "base/logger.h"
#include "render/reources/mesh.h"
#include "tiny_obj_loader.h"
namespace mini::asset{
inline Mesh load_obj(const std::filesystem::path& path){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

    if (!warn.empty()) {
        base::log::warn("{}", warn);
    }

    if (!err.empty()) {
        base::log::error("{}", err);
    }

    if (!success) {
        return {};
    }

    base::log::info("loaded obj : {}", path.c_str());
    
    Mesh mesh;
    std::uint32_t next = 0;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            float vx = attrib.vertices[3 * index.vertex_index + 0];
            float vy = attrib.vertices[3 * index.vertex_index + 1];
            float vz = attrib.vertices[3 * index.vertex_index + 2];
          
            float nx = attrib.vertices[3 * index.vertex_index + 0];
            float ny = attrib.vertices[3 * index.vertex_index + 1];
            float nz = attrib.vertices[3 * index.vertex_index + 2];
         
            glm::vec3 normal = glm::normalize(glm::vec3{nx, ny, nz});
            mesh.vertices_.push_back({{vx, vy, vz}, normal});
            mesh.indices_.push_back(next++);
        }
    }
    return mesh;
}
}
