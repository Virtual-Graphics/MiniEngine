#pragma once
#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace editor {

struct ProjectInfo {
    std::string name;
    std::string description;
    std::string author;

    // relative path to project file
    std::filesystem::path assets_path;
};

bool save_project_info(const std::filesystem::path& project_file, const ProjectInfo& info){
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "name"          << YAML::Value << info.name;
    out << YAML::Key << "description"   << YAML::Value << info.description;
    out << YAML::Key << "author"        << YAML::Value << info.author;
    out << YAML::Key << "assets_path"   << YAML::Value << info.assets_path.string();
    out << YAML::EndMap;

    std::ofstream fout(project_file);
    if (!fout.is_open()) {
        return false;
    }
    fout << out.c_str();
    return true;
}

bool load_project_info(const std::filesystem::path& project_file, ProjectInfo& info){
    YAML::Node node = YAML::LoadFile(project_file.string());
    if (!node.IsMap()) {
        return false;
    }

    info.name        = node["name"].as<std::string>();
    info.description = node["description"].as<std::string>();
    info.author      = node["author"].as<std::string>();
    info.assets_path = node["assets_path"].as<std::string>();

    return true;
}

}

