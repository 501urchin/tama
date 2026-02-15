#pragma once

#include <cstdint>
#include <vector>
#include <span>


enum class status : uint8_t {
    ok,
    emptyParams,
    invalidParam
};








namespace tama {
    /// Stateful Exponential Moving Average (EMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class ExponentialMovingAverage {
    private:
        double lastEma;
        double period;
        double alpha;
        double oma;
        bool initalized = false;
    public:
        /// Creates an EMA indicator instance.
        /// @param period Lookback period used to derive the EMA smoothing factor.
        /// @param prevCalculation Optional previous EMA value used as warm start.
        ExponentialMovingAverage(uint16_t period, double prevCalculation = 0);

        /// Computes EMA values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with EMA values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the EMA with a single new price sample.
        /// @param price New price value.
        /// @return Updated EMA value.
        double update(double price);

        /// Returns the latest EMA value stored by the indicator.
        double latest();
    };

    /// Stateful Simple Moving Average (SMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class SimpleMovingAverage {    
    private:
        double alpha;
        size_t period;
        std::vector<double> priceBuf;
        bool initalized = false;
        double lastSma;
    public:
        /// Creates an SMA indicator instance.
        /// @param period Number of samples used in the SMA window.
        SimpleMovingAverage(uint16_t period, std::vector<double> prevCalc = {});

        /// Computes SMA values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with SMA values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the SMA with a single new price sample.
        /// @param price New price value.
        /// @return Updated SMA value.
        double update(double price);

        /// Returns the latest SMA value stored by the indicator.
        double latest();
    };


    class WeightedMovingAverage {
        private: 
            size_t period;
            double denominator;

            std::vector<double> priceBuf;
            bool initalized = false;
            double lastWma;

        public:
            WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
            status compute(std::span<const double> prices, std::vector<double>& output);
            double update(double price);
            double latest();

    };
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
