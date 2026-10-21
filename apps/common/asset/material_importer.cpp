#include "common/asset/material_importer.h"

#include "asset/material_bin.h"
#include "common/asset/material_file.h"

namespace vulcal::asset_pipeline {

bool import_material(const std::filesystem::path& source, const std::filesystem::path& cache, std::string& error) {
    const auto material = read_material(source, error);
    if (!material) return false;
    return vulcal::asset::encode_material_bin(cache, *material, error);
}

} // namespace vulcal::asset_pipeline
