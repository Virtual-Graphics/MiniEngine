#pragma once
#include <glad/glad.h>

#include "base/logger.h"

namespace mini::gfx {

inline const char* debug_source_str(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API:             return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WindowSystem";
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "ShaderCompiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:     return "ThirdParty";
    case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
    case GL_DEBUG_SOURCE_OTHER:           return "Other";
    default:                              return "Unknown";
    }
}

inline const char* debug_type_str(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DeprecatedBehavior";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UndefinedBehavior";
    case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
    case GL_DEBUG_TYPE_MARKER:              return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:          return "PushGroup";
    case GL_DEBUG_TYPE_POP_GROUP:           return "PopGroup";
    case GL_DEBUG_TYPE_OTHER:               return "Other";
    default:                                return "Unknown";
    }
}

inline void GLAPIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
    const char* src = debug_source_str(source);
    const char* typ = debug_type_str(type);

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        base::log::error("[GL] {} | {} | id={} | {}", src, typ, id, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        base::log::warn("[GL] {} | {} | id={} | {}", src, typ, id, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        base::log::info("[GL] {} | {} | id={} | {}", src, typ, id, message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        base::log::debug("[GL] {} | {} | id={} | {}", src, typ, id, message);
        break;
    default:
        base::log::trace("[GL] {} | {} | id={} | {}", src, typ, id, message);
        break;
    }
}

inline void init_debug_output() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_message_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
}

class DebugScope {
public:
    explicit DebugScope(const char* name) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
    }

    ~DebugScope() { glPopDebugGroup(); }

    DebugScope(const DebugScope&) = delete;
    DebugScope& operator=(const DebugScope&) = delete;
};

inline void set_object_label(GLenum identifier, GLuint id, const char* label) {
    glObjectLabel(identifier, id, -1, label);
}

} // namespace mini::gfx
