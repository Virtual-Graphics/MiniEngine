#pragma once

#include "editor/project/project.h"
#include "base/uuid.h"
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <yaml-cpp/yaml.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace editor::yaml_io {

inline YAML::Node read(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Could not open " + path_to_utf8(path));
    auto document = YAML::Load(input);
    if (!document.IsMap() || document["format_version"].as<int>() != 1)
        throw std::runtime_error("Unsupported or missing format_version in " + path_to_utf8(path));
    return document;
}

// Write beside the destination, then replace it, so failed writes do not truncate existing files.
inline void write(const std::filesystem::path& path, const YAML::Node& document, bool overwrite = true) {
    const auto temporary = path.parent_path() / path_from_utf8(path_to_utf8(path.filename()) + "." + base::UUID::generate().to_string() + ".tmp");
    try {
        YAML::Emitter emitter;
        emitter.SetIndent(2);
        emitter << document;
        if (!emitter.good()) throw std::runtime_error(emitter.GetLastError());
        std::ofstream output;
        output.exceptions(std::ios::badbit | std::ios::failbit);
        output.open(temporary, std::ios::binary | std::ios::trunc);
        output << emitter.c_str() << '\n';
        output.close();
#ifdef _WIN32
        if (!MoveFileExW(temporary.c_str(), path.c_str(), overwrite ? MOVEFILE_REPLACE_EXISTING : 0))
            throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "Could not write " + path_to_utf8(path));
#else
        if (overwrite) std::filesystem::rename(temporary, path);
        else {
            std::filesystem::create_hard_link(temporary, path);
            std::filesystem::remove(temporary);
        }
#endif
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}
} // namespace editor::yaml_io
