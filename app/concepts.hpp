#pragma once

#include <concepts>

namespace cynlr {

template<typename T, T N, T MIN, T MAX>
concept within_range = (N >= MIN && N<= MAX);

template<typename T, T N, T SIZE>
concept larger_than = (N > SIZE);

template<typename T, T N, T SIZE>
concept equal_to = (N == SIZE);

template <typename T, T N>
concept power_of_two = (N > 0) && ((N & (N - 1)) == 0);

template <typename T>
concept enum_class = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

} // namespace cynlr