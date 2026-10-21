#pragma once

#include "render/resources/mesh_data.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace vulcal::asset {

// Import cache layout is defined in one place so the writer and reader cannot drift.
inline constexpr char kMeshMagic[4] = { 'V', 'M', 'S', 'H' };
inline constexpr std::uint32_t kMeshBinVersion = 1;

std::optional<MeshData> decode_mesh_bin(const std::filesystem::path& path);
bool encode_mesh_bin(const std::filesystem::path& path, const MeshData& mesh, std::string& error);

} // namespace vulcal::asset
