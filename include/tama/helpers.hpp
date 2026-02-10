#pragma once

#include <span>
#include <type_traits>

namespace tama::helpers {

float simdSumF32(std::span<const float> elms);

double simdSumF64(std::span<const double> elms);

template <typename T>
T simdSum(std::span<const T> value) {
    if constexpr (std::is_same_v<T, float>) {
        return simdSumF32(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return simdSumF64(value);
    } else {
        static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>,
                      "only float or double is allowed");
    }
}

} // namespace tama::helpers
