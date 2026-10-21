#pragma once

#include <filesystem>
#include <string>

namespace vulcal::asset_pipeline {
// Parses an OBJ source and writes its Core-defined .vmesh cache.
bool import_obj(const std::filesystem::path& source, const std::filesystem::path& cache, std::string& error);
} // namespace vulcal::asset_pipeline
