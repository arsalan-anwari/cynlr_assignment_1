#pragma once

#include "types.hpp"

namespace cynlr {
namespace settings {

    static constexpr usize ProfileBufferSize = 1000u;
    static constexpr usize MinCsvReaderBufferSize = 100u;
    static constexpr u64 RandomSeedU64 = 0xDEADBEEFCAFEBABE;

    namespace task_filter_threshold {
        static constexpr usize MinThresholdBufferSize = 1000u;
    }

    namespace task_data_generator {
        static constexpr usize DeadlineTaskNs = 500u;
        static constexpr usize DeadlineTaskDebugNs = 100'000'000u; // 100ms
    }
    

    
}} // namespace cynlr::settings