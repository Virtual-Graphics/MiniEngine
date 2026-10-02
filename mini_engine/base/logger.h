#pragma once

#include <utility>

#include <spdlog/spdlog.h>

namespace base {
inline void initLogger() {
    spdlog::set_pattern("[%^%l%$] %v");
    spdlog::set_level(spdlog::level::trace);
}

namespace log {

template <typename... Args>
inline void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void critical(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::critical(fmt, std::forward<Args>(args)...);
}

} // namespace log
} // namespace base
