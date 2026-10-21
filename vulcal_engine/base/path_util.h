#pragma once

#include <filesystem>
#include <string>

namespace base {

inline std::string path_to_utf8(const std::filesystem::path& path) {
    const auto text = path.generic_u8string();
    return { reinterpret_cast<const char*>(text.data()), text.size() };
}

inline std::filesystem::path path_from_utf8(const std::string& text) {
    return std::filesystem::path{ std::u8string(text.begin(), text.end()) };
}

} // namespace base
