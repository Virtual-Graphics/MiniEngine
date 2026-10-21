#include "asset/material_bin.h"

#include "base/logger.h"
#include "base/path_util.h"

#include <array>
#include <cmath>
#include <cstring>
#include <fstream>

namespace vulcal::asset {
namespace {
constexpr std::size_t kValueCount = 7;

bool read_header(std::ifstream& input, std::uint32_t& version) {
    std::array<char, 4> magic{};
    if (!input.read(magic.data(), magic.size())) return false;
    if (std::memcmp(magic.data(), kMaterialMagic, magic.size()) != 0) return false;
    return static_cast<bool>(input.read(reinterpret_cast<char*>(&version), sizeof(version)));
}
} // namespace

std::optional<render::Material> decode_material_bin(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        base::log::error("Material cache not found: {}", base::path_to_utf8(path));
        return std::nullopt;
    }

    std::uint32_t version = 0;
    std::array<float, kValueCount> values{};
    if (!read_header(input, version) || version != kMaterialBinVersion ||
            !input.read(reinterpret_cast<char*>(values.data()), sizeof(float) * values.size())) {
        base::log::error("Invalid material cache: {}", base::path_to_utf8(path));
        return std::nullopt;
    }
    for (const float value: values) {
        if (!std::isfinite(value)) {
            base::log::error("Invalid material cache: {}", base::path_to_utf8(path));
            return std::nullopt;
        }
    }

    render::Material material;
    material.diffuse_ = glm::vec3{ values[0], values[1], values[2] };
    material.specular_ = glm::vec3{ values[3], values[4], values[5] };
    material.shininess_ = values[6];
    return material;
}

bool encode_material_bin(const std::filesystem::path& path, const render::Material& material, std::string& error) {
    for (glm::length_t i = 0; i < 3; ++i) {
        if (!std::isfinite(material.diffuse_[i]) || !std::isfinite(material.specular_[i])) {
            error = "Material colors must be finite";
            return false;
        }
    }
    if (!std::isfinite(material.shininess_) || material.shininess_ < 0.0f) {
        error = "Material shininess must be a non-negative number";
        return false;
    }

    std::error_code ignored;
    std::filesystem::create_directories(path.parent_path(), ignored);
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }

    const std::array<float, kValueCount> values{
        material.diffuse_.x,
        material.diffuse_.y,
        material.diffuse_.z,
        material.specular_.x,
        material.specular_.y,
        material.specular_.z,
        material.shininess_,
    };
    output.write(kMaterialMagic, sizeof(kMaterialMagic));
    output.write(reinterpret_cast<const char*>(&kMaterialBinVersion), sizeof(kMaterialBinVersion));
    output.write(reinterpret_cast<const char*>(values.data()), sizeof(float) * values.size());
    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }
    return true;
}

} // namespace vulcal::asset
