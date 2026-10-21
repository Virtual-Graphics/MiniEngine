#pragma once

#include <filesystem>
#include <string>

namespace vulcal::asset_pipeline {
// Parses a .vmat source and writes its Core-defined .vmatc cache.
bool import_material(const std::filesystem::path& source, const std::filesystem::path& cache, std::string& error);
} // namespace vulcal::asset_pipeline
