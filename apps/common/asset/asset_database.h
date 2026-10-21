#pragma once

#include "base/uuid.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace vulcal::asset_pipeline {

enum class AssetKind { Mesh, Material };
enum class AssetStatus { Ready, Failed };

struct AssetRecord {
    base::UUID id_;
    AssetKind kind_ = AssetKind::Mesh;
    AssetStatus status_ = AssetStatus::Failed;
    std::filesystem::path source_;
    std::filesystem::path cache_path_;
    std::string name_;
    std::string error_;
};

// Project-side view of the Asset directory: maintains .meta files, generates missing
// import caches and remembers per-asset errors. It only knows paths, not Editor state.
class AssetDatabase {
public:
    void set_roots(std::filesystem::path asset_root, std::filesystem::path cache_root);
    // Discovers supported sources, creates missing .meta files and caches.
    // Existing caches are not checked against the source; use reimport().
    void rescan();
    bool reimport(base::UUID id, std::string& error);
    bool create_material(const std::string& name, base::UUID& id, std::string& error);

    const std::vector<AssetRecord>& records() const { return records_; }
    const AssetRecord* find(base::UUID id) const;
    const std::filesystem::path& cache_directory() const { return cache_directory_; }
    std::uint64_t revision() const { return revision_; }

private:
    bool import(AssetRecord& record, std::string& error) const;
    base::UUID next_id() const;

    std::filesystem::path assets_root_;
    std::filesystem::path cache_directory_;
    std::vector<AssetRecord> records_;
    std::uint64_t revision_ = 0;
};

} // namespace vulcal::asset_pipeline
