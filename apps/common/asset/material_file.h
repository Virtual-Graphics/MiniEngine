#pragma once

#include "render/resources/material.h"

#include <filesystem>
#include <optional>
#include <string>

namespace vulcal::asset_pipeline {
// Material source files are YAML and remain the authoritative editable format.
std::optional<render::Material> read_material(const std::filesystem::path& path, std::string& error);
bool write_material(const std::filesystem::path& path, const render::Material& material, std::string& error);
} // namespace vulcal::asset_pipeline
