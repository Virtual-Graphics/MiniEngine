#include "common/asset/image_loader.h"

#include "base/logger.h"
#include "base/path_util.h"

#include <stb_image.h>
#include <stdexcept>

namespace vulcal::asset_pipeline {

ImageData load_image(const std::filesystem::path& path, int desired_channels) {
    stbi_set_flip_vertically_on_load(1);

    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, desired_channels);
    if (!data) {
        const char* reason = stbi_failure_reason();
        const auto display = base::path_to_utf8(path);
        base::log::error("Failed to load image: {} ({})", display, reason ? reason : "unknown");
        throw std::runtime_error("Image load failed: " + display);
    }

    if (desired_channels != 0) channels = desired_channels;
    const std::size_t size = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * static_cast<std::size_t>(channels);

    ImageData image;
    image.width = static_cast<std::uint32_t>(width);
    image.height = static_cast<std::uint32_t>(height);
    image.channels = static_cast<std::uint32_t>(channels);
    image.pixels.assign(data, data + size);
    stbi_image_free(data);
    return image;
}

} // namespace vulcal::asset_pipeline
