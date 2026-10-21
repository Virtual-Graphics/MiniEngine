#include "common/asset/obj_importer.h"

#include "asset/mesh_bin.h"
#include "common/asset/obj_loader.h"

namespace vulcal::asset_pipeline {

bool import_obj(const std::filesystem::path& source, const std::filesystem::path& cache, std::string& error) {
    const auto mesh = load_obj(source, error);
    if (!mesh) return false;
    return vulcal::asset::encode_mesh_bin(cache, *mesh, error);
}

} // namespace vulcal::asset_pipeline
