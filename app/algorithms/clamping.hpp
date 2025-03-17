#pragma once

#include "../types.hpp"

namespace cynlr{

    constexpr auto clamp_top(usize dividend, usize divisor) -> usize {
        return (dividend + divisor - 1) / divisor;
    }
    
} // namespace cynlr