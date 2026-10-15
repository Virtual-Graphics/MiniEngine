#pragma once
#include <glad/glad.h>

namespace mini::gfx {

enum class CompareFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

inline GLenum to_gl(CompareFunc func) {
    switch (func) {
    case CompareFunc::Never:        return GL_NEVER;
    case CompareFunc::Less:         return GL_LESS;
    case CompareFunc::Equal:        return GL_EQUAL;
    case CompareFunc::LessEqual:    return GL_LEQUAL;
    case CompareFunc::Greater:      return GL_GREATER;
    case CompareFunc::NotEqual:     return GL_NOTEQUAL;
    case CompareFunc::GreaterEqual: return GL_GEQUAL;
    case CompareFunc::Always:       return GL_ALWAYS;
    }
    return GL_LESS;
}

} // namespace mini::gfx
