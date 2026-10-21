#pragma once

#include "render/resources/mesh_data.h"

#include <filesystem>
#include <optional>
#include <string>

namespace vulcal::asset_pipeline {
// Source-format parsing shared by the Editor importer and the Model Viewer.
// No .meta, project or cache knowledge lives here.
std::optional<MeshData> load_obj(const std::filesystem::path& path, std::string& error);
} // namespace vulcal::asset_pipeline
