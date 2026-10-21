#pragma once

#include "render/resources/material.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace vulcal::asset {

inline constexpr char kMaterialMagic[4] = { 'V', 'M', 'A', 'T' };
inline constexpr std::uint32_t kMaterialBinVersion = 1;

std::optional<render::Material> decode_material_bin(const std::filesystem::path& path);
bool encode_material_bin(const std::filesystem::path& path, const render::Material& material, std::string& error);

} // namespace vulcal::asset
