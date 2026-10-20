#pragma once
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include "base/logger.h"
#include "render/resources/mesh_data.h"
#include "tiny_obj_loader.h"

namespace vulcal::asset {
class OBJImporter {
public:
    static MeshData load_obj(const std::filesystem::path& path) {
        const auto utf8 = path.generic_u8string();
        const std::string display(utf8.begin(), utf8.end());
        // Open through filesystem::path to preserve Unicode filenames on Windows.
        std::ifstream input(path, std::ios::binary);
        if (!input) {
            base::log::warn("Could not open OBJ: {}", display);
            return {};
        }
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, error;
        const bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, &input);
        if (!warn.empty()) base::log::warn("{}", warn);
        if (!error.empty()) base::log::error("{}", error);
        if (!success) return {};

        MeshData mesh;
        for (const auto& shape : shapes) {
            const auto& indices = shape.mesh.indices;
            for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
                glm::vec3 positions[3];
                for (int corner = 0; corner < 3; ++corner) {
                    const auto vertex = indices[i + corner].vertex_index;
                    if (vertex < 0 || static_cast<std::size_t>(vertex) >= attrib.vertices.size() / 3) {
                        base::log::warn("Invalid vertex index in OBJ: {}", display);
                        return {};
                    }
                    const auto offset = static_cast<std::size_t>(vertex) * 3;
                    positions[corner] = { attrib.vertices[offset], attrib.vertices[offset + 1], attrib.vertices[offset + 2] };
                    for (int axis = 0; axis < 3; ++axis) if (!std::isfinite(positions[corner][axis])) return {};
                }
                auto normal = glm::cross(positions[1] - positions[0], positions[2] - positions[0]);
                const auto length = glm::length(normal);
                normal = length > 0 ? normal / length : glm::vec3{ 0, 1, 0 };
                for (int corner = 0; corner < 3; ++corner) {
                    auto vertex_normal = normal;
                    const auto index = indices[i + corner].normal_index;
                    if (index >= 0 && static_cast<std::size_t>(index) < attrib.normals.size() / 3) {
                        const auto offset = static_cast<std::size_t>(index) * 3;
                        vertex_normal = { attrib.normals[offset], attrib.normals[offset + 1], attrib.normals[offset + 2] };
                        for (int axis = 0; axis < 3; ++axis) if (!std::isfinite(vertex_normal[axis])) return {};
                    }
                    mesh.indices_.push_back(static_cast<std::uint32_t>(mesh.vertices_.size()));
                    mesh.vertices_.push_back({ positions[corner], vertex_normal });
                }
            }
        }
        base::log::info("Loaded OBJ: {}", display);
        return mesh;
    }
};
} // namespace vulcal::asset
