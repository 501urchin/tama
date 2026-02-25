#include <cstdint>
#include <vector>
#include <span>
#include <helpers/helpers.hpp>


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
        double lastEma{0.0};
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
        helpers::RingBuffer<double> priceBuf;
        double rollingSum{0.0};
        bool initalized{false};
        double lastSma{0.0};
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

            helpers::RingBuffer<double> priceBuf;

            double rollingSum{0.0};
            double rollingWeightedSum{0.0};

            bool initialized{false};
            double lastWma{0.0};

        public:
            /// Creates a WMA indicator instance.
            /// @param period Number of samples used in the WMA window.
            /// @param prevCalc Optional warm-start buffer containing the latest `period`
            /// prices ordered from past to present (oldest to newest).
            WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc = {});

            /// Computes WMA values for the full input series.
            /// @param prices Input price series.
            /// @param output Output vector resized/written with WMA values.
            /// @return status indicating success or failure.
            status compute(std::span<const double> prices, std::vector<double>& output);

            /// Updates the WMA with a single new price sample.
            /// @param price New price value           /// @return Updated WMA value.
            double update(double price);

            /// Returns the latest WMA value stored by the indicator.
            double latest();

    };

    class VolumeWeightedMovingAverage {
        private: 
            size_t period;
            helpers::RingBuffer<double> priceBuf;
            helpers::RingBuffer<double> volumeBuf;
            
            double rollingNumerator;
            double rollingDenominator;

            bool initialized;
            double lastCalculation;
        public:
            VolumeWeightedMovingAverage(uint16_t period, std::vector<double> prevPrices = {}, std::vector<double> prevVolume = {});
            status compute(std::span<const double> prices, std::span<const double> volume, std::vector<double>& output);
            double update(double price, double volume);
            double latest();
    };



    class HullMovingAverage {
        uint16_t p1;
        uint16_t p2;
        uint16_t period;

        WeightedMovingAverage w1;
        WeightedMovingAverage w2;
        WeightedMovingAverage w3;

        double lastHull = 0.0;
        bool initialized = false;
    public:
        /// Creates an HMA indicator instance.
        /// @param period Base lookback period used by the HMA.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        HullMovingAverage(uint16_t period, std::vector<double> prevCalc = {});

        /// Computes HMA values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with HMA values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the HMA with a single new price sample.
        /// @param price New price value.
        /// @return Updated HMA value.
        double update(double price);

        /// Returns the latest HMA value stored by the indicator.
        double latest();
    };

    /// Stateful Double Exponential Moving Average (DEMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class DoubleExponentialMovingAverage {
    private:
        size_t period;

        ExponentialMovingAverage ema1;
        ExponentialMovingAverage ema2;

        bool initialized{false};
        double lastDema{0.0};

    public:
        /// Creates a DEMA indicator instance.
        /// @param period Lookback period used by the EMA cascade.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        DoubleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc = {});

        /// Computes DEMA values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with DEMA values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the DEMA with a single new price sample.
        /// @param price New price value.
        /// @return Updated DEMA value.
        double update(double price);

        /// Returns the latest DEMA value stored by the indicator.
        double latest();
    };

    /// Stateful Triple Exponential Moving Average (TEMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class TripleExponentialMovingAverage {
    private:
        size_t period;

        ExponentialMovingAverage ema1;
        ExponentialMovingAverage ema2;
        ExponentialMovingAverage ema3;

        bool initialized{false};
        double lastTema{0.0};

    public:
        /// Creates a TEMA indicator instance.
        /// @param period Lookback period used by the EMA cascade.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        TripleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc = {});

        /// Computes TEMA values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with TEMA values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the TEMA with a single new price sample.
        /// @param price New price value.
        /// @return Updated TEMA value.
        double update(double price);

        /// Returns the latest TEMA value stored by the indicator.
        double latest();
    };
    
    /// Stateful McGinley Dynamic (MD) indicator.
    /// Supports both batch computation and single-tick updates.
    class McGinleyDynamicMovingAverage {
    private:
        uint16_t period;
        double lastMd{0.0};
        bool initialized{false};

    public:
        /// Creates an MD indicator instance.
        /// @param period Base lookback period used in the MD calculation.
        /// @param prevCalculation Optional previous MD value used as warm start.
        McGinleyDynamicMovingAverage(uint16_t period, double prevCalculation = 0.0);

        /// Computes MD values for the full input series.
        /// @param prices Input price series.
        /// @param output Output vector resized/written with MD values.
        /// @return status indicating success or failure.
        status compute(std::span<const double> prices, std::vector<double>& output);

        /// Updates the MD with a single new price sample.
        /// @param price New price value.
        /// @return Updated MD value.
        double update(double price);

        /// Returns the latest MD value stored by the indicator.
        double latest();
    };

    class FractalAdaptiveMovingAverage {
    private:
        bool initialized{false};
        size_t period;
        double eulerNumber;
        double halfPeriod;
        double logTwo;

        double highBuf1Max{0.0};
        double highBuf2Max{0.0};
        double lowBuf1min{0.0};
        double lowBuf2min{0.0};

        double lastFrama{0.0};

        helpers::RingBuffer<double> highBuf1;
        helpers::RingBuffer<double> highBuf2;
        helpers::RingBuffer<double> lowBuf1;
        helpers::RingBuffer<double> lowBuf2;

    public:
        FractalAdaptiveMovingAverage(uint16_t period, double eulerNumber = -4.6);
        status compute(std::span<const double> close, std::span<const double> low, std::span<const double> high, std::vector<double>& output);
        double latest();
        double update(double close, double low, double high);
    };
 } // namespace tama



