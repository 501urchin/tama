#pragma once

#include <vector>
#include <cstdint>


enum class status {
    ok,
    emptyPrices,
    invalidParam
};

namespace tama {
    status ema(const std::vector<double>& prices, std::vector<double>& emaOut, const uint16_t emaPeriod);
    status sma(const std::vector<double>& prices, std::vector<double>& smaOut, const uint16_t smaPeriod);
}