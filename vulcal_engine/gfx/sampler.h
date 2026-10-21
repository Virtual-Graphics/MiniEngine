#pragma once
#include "gfx/compare_func.h"
#include "object.h"
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <optional>

namespace vulcal::gfx {

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

        float min_lod = -1000.0f;
        float max_lod = 1000.0f;
        float lod_bias = 0.0f;
        float max_anisotropy = 1.0f;
        glm::vec4 border_color{ 0.0f, 0.0f, 0.0f, 0.0f };
        std::optional<CompareFunc> compare_func;

        static Spec repeat() { return Spec{}; }

        static Spec clamp_to_edge() {
            Spec spec;
            spec.wrap_s = Wrap::ClampToEdge;
            spec.wrap_t = Wrap::ClampToEdge;
            spec.wrap_r = Wrap::ClampToEdge;
            return spec;
        }
    };

    explicit Sampler(const Spec& spec);

    void bind(std::uint32_t unit) const { glBindSampler(unit, id_); }

private:
    static GLenum to_gl(Filter filter);
    static GLenum to_gl(Wrap wrap);
};

}
