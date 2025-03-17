#pragma once

#include <concepts>
#include "../types.hpp"
#include "../settings.hpp"

namespace cynlr {


    enum class random_engine {
        xor_shift, lcg, split_mix, pcg, mersenne_twist 
    };

    template<random_engine engine, typename T> requires (std::integral<T> || std::floating_point<T>)
    struct random;

    template<> struct random<random_engine::xor_shift, u64> {
        auto operator()() -> u64 {
            seed ^= seed >> 12;  // Right shift and XOR
            seed ^= seed << 25;  // Left shift and XOR
            seed ^= seed >> 27;  // Right shift and XOR
            return seed * 2685821657736338717ULL;  // Multiply by a large prime
        }
    private:
        static inline u64 seed = settings::RandomSeedU64;
    };

    template<> struct random<random_engine::xor_shift, u8> {
        auto operator()() -> u8 {
            return static_cast<u8>(random<random_engine::xor_shift, u64>()() & 0xFF);   
        }
    };



} // namespace cynlr