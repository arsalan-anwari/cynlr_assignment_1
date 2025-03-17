#pragma once

#include <format>
#include <span>

#include "../types.hpp"

namespace cynlr {

template<typename T>
auto join_str(std::span<const T> in, char sep = ' ') -> str {
    str msg;

    for (const auto& e : in) {
        msg += std::format("{} {}", e, sep);
    }

    msg.pop_back();
    msg.pop_back();

    return msg;
};

} // namespace cynlr