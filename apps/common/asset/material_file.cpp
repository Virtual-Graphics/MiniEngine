#include "common/asset/material_file.h"

#include "base/path_util.h"

#include <cmath>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace vulcal::asset_pipeline {
namespace {
constexpr int kMaterialFormatVersion = 1;

bool read_color(const YAML::Node& node, const char* key, glm::vec3& color, std::string& error) {
    const auto data = node[key];
    if (!data) return true;
    if (!data.IsSequence() || data.size() != 3) {
        error = std::string("Material field '") + key + "' must contain three numbers";
        return false;
    }
    for (glm::length_t i = 0; i < 3; ++i) {
        try {
            color[i] = data[static_cast<std::size_t>(i)].as<float>();
        } catch (const std::exception&) {
            error = std::string("Material field '") + key + "' contains an invalid number";
            return false;
        }
        if (!std::isfinite(color[i])) {
            error = std::string("Material field '") + key + "' contains a non-finite number";
            return false;
        }
    }
    return true;
}
} // namespace

std::optional<render::Material> read_material(const std::filesystem::path& path, std::string& error) {
    try {
        std::ifstream input(path, std::ios::binary);
        if (!input) {
            error = "Could not open " + base::path_to_utf8(path);
            return std::nullopt;
        }
        const auto root = YAML::Load(input);
        if (!root.IsMap() || root["format_version"].as<int>() != kMaterialFormatVersion) {
            error = "Unsupported material format: " + base::path_to_utf8(path);
            return std::nullopt;
        }

        render::Material material;
        if (!read_color(root, "diffuse", material.diffuse_, error) || !read_color(root, "specular", material.specular_, error)) {
            return std::nullopt;
        }
        if (const auto shininess = root["shininess"]) {
            material.shininess_ = shininess.as<float>();
            if (!std::isfinite(material.shininess_) || material.shininess_ < 0.0f) {
                error = "Material shininess must be a non-negative number";
                return std::nullopt;
            }
        }
        return material;
    } catch (const std::exception& exception) {
        error = std::string(base::path_to_utf8(path)) + ": " + exception.what();
        return std::nullopt;
    }
}

bool write_material(const std::filesystem::path& path, const render::Material& material, std::string& error) {
    if (!std::isfinite(material.shininess_) || material.shininess_ < 0.0f) {
        error = "Material shininess must be a non-negative number";
        return false;
    }
    for (glm::length_t i = 0; i < 3; ++i) {
        if (!std::isfinite(material.diffuse_[i]) || !std::isfinite(material.specular_[i])) {
            error = "Material colors must be finite";
            return false;
        }
    }

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "format_version" << YAML::Value << kMaterialFormatVersion;
    const auto emit_color = [&out](const char* key, const glm::vec3& color) {
        out << YAML::Key << key << YAML::Value << YAML::Flow << YAML::BeginSeq;
        for (glm::length_t i = 0; i < 3; ++i) out << color[i];
        out << YAML::EndSeq;
    };
    emit_color("diffuse", material.diffuse_);
    emit_color("specular", material.specular_);
    out << YAML::Key << "shininess" << YAML::Value << material.shininess_;
    out << YAML::EndMap;
    if (!out.good()) {
        error = "Could not serialize material";
        return false;
    }

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }
    output << out.c_str() << '\n';
    if (!output) {
        error = "Could not write " + base::path_to_utf8(path);
        return false;
    }
    return true;
}

} // namespace vulcal::asset_pipeline
