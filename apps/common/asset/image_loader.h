#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace vulcal::asset_pipeline {

struct ImageData {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t channels = 0;
    std::vector<std::uint8_t> pixels;
};

// Moved out of Core for a future Texture pipeline. Texture assets are not imported yet.
// Desired channels: 0 keeps the file's own channel count.
ImageData load_image(const std::filesystem::path& path, int desired_channels = 0);

} // namespace vulcal::asset_pipeline
