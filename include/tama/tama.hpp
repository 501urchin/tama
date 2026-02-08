#pragma once

#include <vector>
#include <cstdint>


namespace tama {
    void ema(const std::vector<double>& prices, std::vector<double>& ema, uint16_t emaPeriod);
    void sma(const std::vector<double>& prices, std::vector<double>& sma, uint16_t smaPeriod);
}