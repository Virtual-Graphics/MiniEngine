#pragma once
#include "object.h"
#include <cstdint>
#include <glad/glad.h>

namespace mini::gfx {

class Sampler : public Object {
public:
    enum class Filter {
        Nearest,
        Linear,
        NearestMipmapNearest,  
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear,
    };

    enum class Wrap {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
    };

    struct Spec {
        Filter min_filter = Filter::Linear;
        Filter mag_filter = Filter::Linear;
        Wrap   wrap_s = Wrap::Repeat;       // u dir wrap
        Wrap   wrap_t = Wrap::Repeat;       // v dir wrap
        Wrap   wrap_r = Wrap::Repeat;       // 3d image z dir wrap
    };

    explicit Sampler(const Spec& spec);

    void bind(std::uint32_t unit) const { glBindSampler(unit, id_); }

private:
    static GLenum to_gl(Filter filter);
    static GLenum to_gl(Wrap wrap);
};

}
