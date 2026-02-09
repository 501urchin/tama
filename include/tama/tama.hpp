#pragma once

#include <vector>
#include <cstdint>


namespace tama {
    enum class status : uint8_t {
        ok,
        emptyPrices,
        invalidParam
    };
    
    /// Calculates the Exponential Moving Average (EMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param emaOut Output vector that will contain the EMA values.
    /// @param emaPeriod The period over which to calculate the EMA.
    /// @return status indicating success or failure.
    status ema(const std::vector<double>& prices, std::vector<double>& emaOut, const uint16_t emaPeriod);

    /// Calculates the Simple Moving Average (SMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param smaOut Output vector that will contain the SMA values.
    /// @param smaPeriod The period over which to calculate the SMA.
    /// @return status indicating success or failure.
    status sma(const std::vector<double>& prices, std::vector<double>& smaOut, const uint16_t smaPeriod);

    /// Calculates the Weighted Moving Average (WMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param wmaOut Output vector that will contain the WMA values.
    /// @param wmaPeriod The period over which to calculate the WMA.
    /// @return status indicating success or failure.
    status wma(const std::vector<double>& prices, std::vector<double>& wmaOut, const uint16_t wmaPeriod);
} // namespace tama
