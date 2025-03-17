#pragma once

#include <span>
#include <immintrin.h>

#include "../types.hpp"


namespace cynlr {
    
    template<std::integral T, usize swap_size>
    auto copy_tail_to_head(std::span<T> data, usize additional_back_offset) -> void {
        usize buffer_size = data.size();
        for(usize i = 0; i < swap_size; i++){
            data[i] = data[buffer_size - swap_size - additional_back_offset + i];
        }
    }

    template<> auto copy_tail_to_head<u8, 8u>(std::span<u8> data, usize additional_back_offset) -> void {
        // Load only the last 8 bytes
        __m128i last8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&data[data.size() - 8u - additional_back_offset]));
    
        // Store them into the first 8 bytes
        _mm_storel_epi64(reinterpret_cast<__m128i*>(&data[0]), last8);
    }




} // namespace cynlr