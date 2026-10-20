#include "editor/serialization/project_serializer.h"
#include "editor/serialization/yaml_io.h"

#include <algorithm>
#include <cctype>

namespace editor {
namespace fs = std::filesystem;

std::string path_to_utf8(const fs::path& path) {
    const auto text = path.generic_u8string();
    return { reinterpret_cast<const char*>(text.data()), text.size() };
}

fs::path path_from_utf8(const std::string& text) { return fs::path{ std::u8string(text.begin(), text.end()) }; }

void validate_relative_path(const fs::path& path, bool allow_empty) {
    if (path.empty() && allow_empty) return;
    const auto normal = path.lexically_normal();
    if (path.empty() || path.has_root_path() || normal == "." || normal.empty() || *normal.begin() == "..")
        throw std::invalid_argument("Expected a path within the project: " + path_to_utf8(path));
}

fs::path Project::relative_asset_path(const fs::path& path) const {
    if (path.empty()) throw std::invalid_argument("Asset path is empty");
    const auto absolute = fs::weakly_canonical(path.is_absolute() ? path : resolve(path));
    const auto asset_root = fs::weakly_canonical(resolve(asset_directory_));
    const auto within_assets = absolute.lexically_relative(asset_root);
    validate_relative_path(within_assets);
    const auto relative = absolute.lexically_relative(fs::weakly_canonical(root_directory()));
    validate_relative_path(relative);
    return relative;
}

namespace {
void validate(const Project& project) {
    if (project.name_.empty() || std::all_of(project.name_.begin(), project.name_.end(), [](unsigned char c) { return std::isspace(c); }) ||
            project.name_ == "." || project.name_ == ".." || project.name_.find_first_of("<>:\"/\\|?*\n\r\t") != std::string::npos ||
            project.name_.back() == '.' || project.name_.back() == ' ')
        throw std::invalid_argument("Enter a valid project name (without filename separators)");
    validate_relative_path(project.asset_directory_);
    const auto asset_relative = fs::weakly_canonical(project.resolve(project.asset_directory_))
        .lexically_relative(fs::weakly_canonical(project.root_directory()));
    validate_relative_path(asset_relative);
    if (!project.last_scene_.empty()) {
        validate_relative_path(project.last_scene_);
        project.relative_asset_path(project.last_scene_);
        if (project.last_scene_.extension() != ".vscene") throw std::invalid_argument("last_scene must be a .vscene file");
    }
}

YAML::Node encode(const Project& project) {
    YAML::Node root;
    root["format_version"] = 1;
    auto data = root["project"];
    data["name"] = project.name_;
    data["author"] = project.author_;
    data["asset_directory"] = path_to_utf8(project.asset_directory_.lexically_normal());
    data["last_scene"] = path_to_utf8(project.last_scene_.lexically_normal());
    return root;
}
} // namespace

Project ProjectSerializer::create(const fs::path& directory, const std::string& name, const std::string& author,
        const fs::path& asset_directory) {
    if (directory.empty() || !fs::is_directory(directory)) throw std::invalid_argument("Select an existing project directory");
    Project project{ name, author, asset_directory, {}, fs::weakly_canonical(directory) / path_from_utf8(name + ".vproj") };
    validate(project);
    for (const auto& entry : fs::directory_iterator(project.root_directory())) {
        auto extension = path_to_utf8(entry.path().extension());
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (extension == ".vproj") throw std::runtime_error("The selected directory already contains a .vproj project");
    }
    fs::create_directories(project.resolve(project.asset_directory_));
    yaml_io::write(project.file_path_, encode(project), false);
    return project;
}

Project ProjectSerializer::load(const fs::path& path) {
    if (path.extension() != ".vproj") throw std::invalid_argument("Select a .vproj file");
    const auto root = yaml_io::read(path);
    const auto data = root["project"];
    if (!data.IsMap()) throw std::runtime_error("Project information is missing");
    Project project{ data["name"].as<std::string>(), data["author"].as<std::string>(""),
        path_from_utf8(data["asset_directory"].as<std::string>()), path_from_utf8(data["last_scene"].as<std::string>("")),
        fs::weakly_canonical(path) };
    validate(project);
    return project;
}

void ProjectSerializer::save(const Project& project) {
    validate(project);
    yaml_io::write(project.file_path_, encode(project));
}
} // namespace editor
