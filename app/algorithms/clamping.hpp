#pragma once

#include "../types.hpp"

namespace cynlr{

    constexpr auto clamp_top(usize dividend, usize divisor) -> usize {
        if (divisor == 0) return 0;
        return (dividend + divisor - 1) / divisor;
    }
    
} // namespace cynlr