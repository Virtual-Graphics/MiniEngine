#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace base {

class UUID {
public:
    UUID() = default;
    explicit UUID(std::uint64_t value);

    static UUID generate();

    std::uint64_t value() const { return value_; }
    bool is_nil() const { return value_ == 0; }
    std::string to_string() const;

    bool operator==(const UUID&) const = default;

private:
    std::uint64_t value_{ 0 };
};

} // namespace base

namespace std {
template<>
struct hash<base::UUID> {
    std::size_t operator()(const base::UUID& uuid) const noexcept { return std::hash<std::uint64_t>{}(uuid.value()); }
};
} // namespace std
