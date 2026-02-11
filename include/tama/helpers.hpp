#pragma once

#include <span>

namespace tama::helpers {
    double simdSum(std::span<const double> elms);
} // namespace tama::helpers
