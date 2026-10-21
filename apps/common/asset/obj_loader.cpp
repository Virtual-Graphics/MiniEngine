#include "common/asset/obj_loader.h"

#include "base/logger.h"
#include "base/path_util.h"

#include <cmath>
#include <fstream>
#include <tiny_obj_loader.h>

namespace vulcal::asset_pipeline {

std::optional<MeshData> load_obj(const std::filesystem::path& path, std::string& error) {
    const auto display = base::path_to_utf8(path);
    // Open through filesystem::path to preserve Unicode filenames on Windows.
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        error = "Could not open " + display;
        return std::nullopt;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string parse_error;
    const bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &parse_error, &input);
    if (!warn.empty()) base::log::warn("{}", warn);
    if (!success) {
        error = parse_error.empty() ? "Could not parse " + display : parse_error;
        return std::nullopt;
    }

    MeshData mesh;
    for (const auto& shape: shapes) {
        const auto& indices = shape.mesh.indices;
        for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
            glm::vec3 positions[3];
            for (int corner = 0; corner < 3; ++corner) {
                const auto vertex = indices[i + corner].vertex_index;
                if (vertex < 0 || static_cast<std::size_t>(vertex) >= attrib.vertices.size() / 3) {
                    error = "Invalid vertex index in " + display;
                    return std::nullopt;
                }
                const auto offset = static_cast<std::size_t>(vertex) * 3;
                positions[corner] = { attrib.vertices[offset], attrib.vertices[offset + 1], attrib.vertices[offset + 2] };
                for (int axis = 0; axis < 3; ++axis) {
                    if (!std::isfinite(positions[corner][axis])) {
                        error = "Non-finite position in " + display;
                        return std::nullopt;
                    }
                }
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
                    for (int axis = 0; axis < 3; ++axis) {
                        if (!std::isfinite(vertex_normal[axis])) {
                            error = "Non-finite normal in " + display;
                            return std::nullopt;
                        }
                    }
                }
                mesh.indices_.push_back(static_cast<std::uint32_t>(mesh.vertices_.size()));
                mesh.vertices_.push_back({ positions[corner], vertex_normal });
            }
        }
    }

    if (mesh.vertices_.empty()) {
        error = "OBJ contains no triangles: " + display;
        return std::nullopt;
    }
    base::log::info("Loaded OBJ: {}", display);
    return mesh;
}

} // namespace vulcal::asset_pipeline
