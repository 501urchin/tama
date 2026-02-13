#pragma once

#include <cstdint>
#include <vector>
#include <span>


namespace tama {
    enum class status : uint8_t {
        ok,
        emptyParams,
        invalidParam
    };
    
    /// Calculates the Exponential Moving Average (EMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param emaOut Output vector that will contain the EMA values.
    /// @param emaPeriod The period over which to calculate the EMA.
    /// @return status indicating success or failure.
    status ema(std::span<const double>  prices, std::vector<double>& emaOut, const uint16_t emaPeriod);

    /// Calculates the Simple Moving Average (SMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param smaOut Output vector that will contain the SMA values.
    /// @param smaPeriod The period over which to calculate the SMA.
    /// @return status indicating success or failure.
    status sma(std::span<const double>  prices, std::vector<double>& smaOut, const uint16_t smaPeriod);

    /// Calculates the Weighted Moving Average (WMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param wmaOut Output vector that will contain the WMA values.
    /// @param wmaPeriod The period over which to calculate the WMA.
    /// @return status indicating success or failure.
    status wma(std::span<const double>  prices, std::vector<double>& wmaOut, const uint16_t wmaPeriod);
    
    /// Calculates the Volume-Weighted Moving Average (VWMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param volume Input vector of volumes aligned with prices.
    /// @param vwmaOut Output vector that will contain the VWMA values.
    /// @param vwmaPeriod The period over which to calculate the VWMA.
    /// @return status indicating success or failure.
    status vwma(std::span<const double> prices, std::span<const double> volume, std::vector<double>& vwmaOut, const uint16_t vwmaPeriod);

    /// Calculates the Double Exponential Moving Average (DEMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param demaOut Output vector that will contain the DEMA values.
    /// @param demaPeriod The period over which to calculate the DEMA.
    /// @return status indicating success or failure.
    status dema(std::span<const double> prices, std::vector<double>& demaOut, const uint16_t demaPeriod);

    /// Calculates the Triple Exponential Moving Average (TEMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param temaOut Output vector that will contain the TEMA values.
    /// @param temaPeriod The period over which to calculate the TEMA.
    /// @return status indicating success or failure.
    status tema(std::span<const double> prices, std::vector<double>& temaOut, const uint16_t temaPeriod);
    
    /// Calculates the Hull Moving Average (HMA) of a price series.
    /// @param prices Input vector of prices (Close, Open, High, Low).
    /// @param hullOut Output vector that will contain the HMA values.
    /// @param hullPeriod The period over which to calculate the HMA.
    /// @return status indicating success or failure.
    status hull(std::span<const double> prices, std::vector<double>& hullOut, const uint16_t hullPeriod);

    /// Calculates the McGinley Dynamic (MD), an adaptive moving average that
    /// adjusts its speed based on market volatility to reduce whipsaws.
    /// @param prices Input vector of prices (typically Close).
    /// @param mdOut Output vector that will contain the McGinley Dynamic values.
    /// @param mdPeriod The base lookback period used in the MD calculation.
    /// @return status indicating success or failure.
    status md(std::span<const double> prices, std::vector<double>& mdOut, uint16_t mdPeriod);
 } // namespace tama
