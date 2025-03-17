#pragma once

#include "../types.hpp"

namespace cynlr {

template <typename>
struct is_csize : std::false_type {};

template <std::size_t N>
struct is_csize<csize<N>> : std::true_type {};

// MSVC cannot distinguish between isize/i64 and usize/u64 so use constexpr if 
struct type_format_trait {
    template <typename T>
    struct type_info {
        static constexpr auto name() -> cstr {
            if constexpr (std::is_same_v<T, i8>) return "i8";
            else if constexpr (std::is_same_v<T, i16>) return "i16";
            else if constexpr (std::is_same_v<T, i32>) return "i32";
            else if constexpr (std::is_same_v<T, i64>) return "i64";
            else if constexpr (std::is_same_v<T, isize>) return "isize";

            else if constexpr (std::is_same_v<T, u8>) return "u8";
            else if constexpr (std::is_same_v<T, u16>) return "u16";
            else if constexpr (std::is_same_v<T, u32>) return "u32";
            else if constexpr (std::is_same_v<T, u64>) return "u64";
            else if constexpr (std::is_same_v<T, usize>) return "usize";
            else if constexpr (is_csize<T>::value) return "csize";

            else if constexpr (std::is_same_v<T, f32>) return "f32";
            else if constexpr (std::is_same_v<T, f64>) return "f64";

            else if constexpr (std::is_same_v<T, str>) return "str";
            else if constexpr (std::is_same_v<T, cstr>) return "cstr";

            else if constexpr (std::is_same_v<T, bool>) return "bool";

            else return "any";
        }
    };
};

} // namespace cynlr