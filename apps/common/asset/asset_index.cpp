#include "common/asset/asset_index.h"

#include "base/parse.h"
#include "base/path_util.h"
#include "common/asset/material_file.h"
#include "common/asset/material_importer.h"
#include "common/asset/obj_importer.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <yaml-cpp/yaml.h>

namespace vulcal::asset_pipeline {
namespace fs = std::filesystem;
namespace {

bool supported_source(const fs::path& path, AssetKind& kind) {
    auto extension = base::path_to_utf8(path.extension());
    std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (extension == ".obj") {
        kind = AssetKind::Mesh;
        return true;
    }
    if (extension == ".vmat") {
        kind = AssetKind::Material;
        return true;
    }
    return false;
}

const char* cache_extension(AssetKind kind) {
    switch (kind) {
        case AssetKind::Mesh:
            return ".vmesh";
        case AssetKind::Material:
            return ".vmatc";
    }
    return "";
}

const char* importer_name(AssetKind kind) {
    switch (kind) {
        case AssetKind::Mesh:
            return "obj";
        case AssetKind::Material:
            return "material";
    }
    return "";
}

AssetKind importer_kind(const std::string& name) {
    if (name == "obj") return AssetKind::Mesh;
    if (name == "material") return AssetKind::Material;
    throw std::runtime_error("Unknown importer: " + name);
}

void write_meta(const fs::path& path, const AssetMetaInfo& meta) {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "format_version" << YAML::Value << 1;
    out << YAML::Key << "id" << YAML::Value << meta.id_.value();
    out << YAML::Key << "importer" << YAML::Value << importer_name(meta.kind_);
    out << YAML::EndMap;
    if (!out.good()) throw std::runtime_error("Could not serialize " + base::path_to_utf8(path));

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) throw std::runtime_error("Could not write " + base::path_to_utf8(path));
    output << out.c_str() << '\n';
    if (!output) throw std::runtime_error("Could not write " + base::path_to_utf8(path));
}

AssetMetaInfo read_meta(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Could not open " + base::path_to_utf8(path));
    const auto document = YAML::Load(input);
    if (!document.IsMap() || document["format_version"].as<int>() != 1)
        throw std::runtime_error("Unsupported or missing format_version in " + base::path_to_utf8(path));
    const auto id = document["id"];
    if (!id || !id.IsScalar()) throw std::runtime_error("Invalid asset id in " + base::path_to_utf8(path));
    const auto value = base::parse_uint64(id.Scalar());
    if (!value || *value == 0) throw std::runtime_error("Invalid asset id in " + base::path_to_utf8(path));

    const auto importer = document["importer"];
    if (!importer || !importer.IsScalar()) throw std::runtime_error("Missing importer in " + base::path_to_utf8(path));
    return AssetMetaInfo{ base::UUID{ *value }, importer_kind(importer.Scalar()) };
}

bool valid_material_name(const std::string& name) {
    if (name.empty() || name == "." || name == "..") return false;
    if (name.find_first_of("<>:\"/\\|?*\n\r\t") != std::string::npos) return false;
    if (name.back() == '.' || name.back() == ' ') return false;
    return std::any_of(name.begin(), name.end(), [](unsigned char c) { return !std::isspace(c); });
}

} // namespace

void AssetIndex::set_roots(fs::path asset_root, fs::path cache_root) {
    assets_root_ = std::move(asset_root);
    cache_directory_ = std::move(cache_root);
    records_.clear();
    rescan();
}

void AssetIndex::rescan() {
    records_.clear();
    ++revision_;
    if (assets_root_.empty()) return;

    std::unordered_set<std::uint64_t> used_ids;
    std::error_code error;
    for (fs::recursive_directory_iterator entry(assets_root_, fs::directory_options::skip_permission_denied, error), end; entry != end;
            entry.increment(error)) {
        if (error) {
            error.clear();
            continue;
        }
        const auto path = entry->path();
        std::error_code type_error;
        if (entry->is_directory(type_error)) {
            const auto name = path.filename().string();
            if (!name.empty() && name.front() == '.') entry.disable_recursion_pending();
            continue;
        }

        AssetKind kind = AssetKind::Mesh;
        if (!supported_source(path, kind)) continue;

        AssetRecord record;
        record.kind_ = kind;
        record.source_ = path;
        record.name_ = base::path_to_utf8(path.filename());

        const auto meta_path = base::path_from_utf8(base::path_to_utf8(path) + ".meta");
        try {
            if (fs::exists(meta_path)) {
                const auto meta = read_meta(meta_path);
                if (meta.kind_ != record.kind_) throw std::runtime_error("Importer does not match the source type");
                record.id_ = meta.id_;
                if (!used_ids.insert(record.id_.value()).second) {
                    record.error_ = "Duplicate asset id; the .meta was copied from another source";
                    records_.push_back(std::move(record));
                    continue;
                }
            } else {
                record.id_ = next_id();
                while (used_ids.contains(record.id_.value())) record.id_ = base::UUID::generate();
                used_ids.insert(record.id_.value());
                write_meta(meta_path, AssetMetaInfo{ record.id_, kind });
            }
        } catch (const std::exception& exception) {
            record.status_ = AssetStatus::Failed;
            record.error_ = exception.what();
            records_.push_back(std::move(record));
            continue;
        }

        record.cache_path_ = cache_directory_ / base::path_from_utf8(std::to_string(record.id_.value()) + cache_extension(kind));
        if (fs::exists(record.cache_path_)) {
            record.status_ = AssetStatus::Ready;
        } else {
            std::string import_error;
            if (import(record, import_error)) {
                record.status_ = AssetStatus::Ready;
            } else {
                record.status_ = AssetStatus::Failed;
                record.error_ = std::move(import_error);
            }
        }
        records_.push_back(std::move(record));
    }

    std::sort(records_.begin(), records_.end(),
            [](const AssetRecord& left, const AssetRecord& right) { return left.source_ < right.source_; });
}

bool AssetIndex::import(AssetRecord& record, std::string& error) const {
    switch (record.kind_) {
        case AssetKind::Mesh:
            return import_obj(record.source_, record.cache_path_, error);
        case AssetKind::Material:
            return import_material(record.source_, record.cache_path_, error);
    }
    return false;
}

base::UUID AssetIndex::next_id() const {
    auto id = base::UUID::generate();
    while (find(id)) id = base::UUID::generate();
    return id;
}

const AssetRecord* AssetIndex::find(base::UUID id) const {
    if (id.is_nil()) return nullptr;
    for (const auto& record: records_)
        if (record.id_ == id) return &record;
    return nullptr;
}

bool AssetIndex::reimport(base::UUID id, std::string& error) {
    for (auto& record: records_) {
        if (record.id_ != id || id.is_nil()) continue;
        if (!import(record, error)) {
            record.status_ = AssetStatus::Failed;
            record.error_ = error;
            return false;
        }
        record.status_ = AssetStatus::Ready;
        record.error_.clear();
        ++revision_;
        return true;
    }
    error = "Unknown asset";
    return false;
}

bool AssetIndex::create_material(const std::string& name, base::UUID& id, std::string& error) {
    if (assets_root_.empty()) {
        error = "No project is open";
        return false;
    }
    if (!valid_material_name(name)) {
        error = "Enter a material name without file separators";
        return false;
    }

    const auto source = assets_root_ / base::path_from_utf8(name + ".vmat");
    const auto meta = base::path_from_utf8(base::path_to_utf8(source) + ".meta");
    if (fs::exists(source) || fs::exists(meta)) {
        error = "A material with that name already exists";
        return false;
    }
    fs::create_directories(assets_root_);
    if (!write_material(source, vulcal::render::Material{}, error)) return false;

    id = next_id();
    try {
        write_meta(meta, AssetMetaInfo{ id, AssetKind::Material });
    } catch (const std::exception& exception) {
        error = exception.what();
        return false;
    }
    rescan();
    if (const auto* record = find(id); record && record->status_ == AssetStatus::Ready) return true;
    error = "The material was created but its import cache could not be generated";
    return false;
}

} // namespace vulcal::asset_pipeline
