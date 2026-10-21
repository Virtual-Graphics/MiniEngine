#pragma once

#include <charconv>
#include <cstdint>
#include <optional>
#include <string_view>

namespace base {

inline std::optional<std::uint64_t> parse_uint64(std::string_view text) {
    std::uint64_t value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) return std::nullopt;
    return value;
}

} // namespace base
