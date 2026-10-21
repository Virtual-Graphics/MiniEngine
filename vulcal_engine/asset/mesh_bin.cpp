#include "asset/mesh_bin.h"

#include "base/logger.h"
#include "base/path_util.h"

#include <array>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>

namespace vulcal::asset {
namespace {
// Guards against corrupt counts requesting a huge allocation before any data is read.
constexpr std::uint32_t kMaxVertices = 8'000'000;
constexpr std::uint32_t kMaxIndices = 24'000'000;

template<typename T>
bool read_value(std::ifstream& input, T& value) {
    return static_cast<bool>(input.read(reinterpret_cast<char*>(&value), sizeof(T)));
}

template<typename T>
void write_value(std::ofstream& output, const T& value) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

bool read_header(std::ifstream& input, std::uint32_t& version) {
    std::array<char, 4> magic{};
    if (!input.read(magic.data(), magic.size())) return false;
    if (std::memcmp(magic.data(), kMeshMagic, magic.size()) != 0) return false;
    return read_value(input, version);
}
} // namespace

std::optional<MeshData> decode_mesh_bin(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        base::log::error("Mesh cache not found: {}", base::path_to_utf8(path));
        return std::nullopt;
    }

    std::uint32_t version = 0;
    std::uint32_t vertex_count = 0;
    std::uint32_t index_count = 0;
    if (!read_header(input, version) || version != kMeshBinVersion || !read_value(input, vertex_count) || !read_value(input, index_count) ||
            vertex_count > kMaxVertices || index_count > kMaxIndices) {
        base::log::error("Invalid mesh cache: {}", base::path_to_utf8(path));
        return std::nullopt;
    }

    MeshData mesh;
    mesh.vertices_.reserve(vertex_count);
    for (std::uint32_t i = 0; i < vertex_count; ++i) {
        Vertex vertex;
        if (!read_value(input, vertex.position.x) || !read_value(input, vertex.position.y) || !read_value(input, vertex.position.z) ||
                !read_value(input, vertex.normal.x) || !read_value(input, vertex.normal.y) || !read_value(input, vertex.normal.z)) {
            base::log::error("Truncated mesh cache: {}", base::path_to_utf8(path));
            return std::nullopt;
        }
        for (int axis = 0; axis < 3; ++axis) {
            if (!std::isfinite(vertex.position[axis]) || !std::isfinite(vertex.normal[axis])) {
                base::log::error("Invalid mesh cache: {}", base::path_to_utf8(path));
                return std::nullopt;
            }
        }
        mesh.vertices_.push_back(vertex);
    }

    mesh.indices_.reserve(index_count);
    for (std::uint32_t i = 0; i < index_count; ++i) {
        std::uint32_t index = 0;
        if (!read_value(input, index) || index >= vertex_count) {
            base::log::error("Invalid mesh cache: {}", base::path_to_utf8(path));
            return std::nullopt;
        }
        mesh.indices_.push_back(index);
    }
    return mesh;
}

bool encode_mesh_bin(const std::filesystem::path& path, const MeshData& mesh, std::string& error) {
    if (mesh.vertices_.empty() || mesh.indices_.empty()) {
        error = "Mesh has no vertices or indices";
        return false;
    }
    if (mesh.vertices_.size() > std::numeric_limits<std::uint32_t>::max() ||
            mesh.indices_.size() > std::numeric_limits<std::uint32_t>::max()) {
        error = "Mesh is too large";
        return false;
    }

    std::error_code ignored;
    std::filesystem::create_directories(path.parent_path(), ignored);
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }

    output.write(kMeshMagic, sizeof(kMeshMagic));
    write_value(output, kMeshBinVersion);
    write_value(output, static_cast<std::uint32_t>(mesh.vertices_.size()));
    write_value(output, static_cast<std::uint32_t>(mesh.indices_.size()));
    for (const auto& vertex: mesh.vertices_) {
        write_value(output, vertex.position.x);
        write_value(output, vertex.position.y);
        write_value(output, vertex.position.z);
        write_value(output, vertex.normal.x);
        write_value(output, vertex.normal.y);
        write_value(output, vertex.normal.z);
    }
    for (const auto index: mesh.indices_) write_value(output, index);

    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }
    return true;
}

} // namespace vulcal::asset
