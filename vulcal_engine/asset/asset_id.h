#pragma once

#include "base/uuid.h"

namespace vulcal::asset {

// Persistent identities of imported assets. They share the UUID representation but are
// distinct types, so a material ID cannot be assigned where a mesh ID is expected.
struct MeshID {
    base::UUID value_;
    bool is_nil() const { return value_.is_nil(); }
    bool operator==(const MeshID&) const = default;
};

struct MaterialID {
    base::UUID value_;
    bool is_nil() const { return value_.is_nil(); }
    bool operator==(const MaterialID&) const = default;
};

} // namespace vulcal::asset
