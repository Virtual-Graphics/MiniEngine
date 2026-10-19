#pragma once
#include "image.h"
#include "sampler.h"
#include <cstdint>

namespace vulcal::gfx {

class Texture {
public:
    Texture(Image image, Sampler sampler)
            : image_(std::move(image)),
              sampler_(std::move(sampler)) {}

    void bind(std::uint32_t unit) const {
        image_.bind(unit);
        sampler_.bind(unit);
    }

    const Image& image() const { return image_; }
    const Sampler& sampler() const { return sampler_; }

private:
    Image image_;
    Sampler sampler_;
};

}
