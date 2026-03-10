#include <cstdint>
#include <vector>
#include <span>
#include <helpers/helpers.hpp>


enum class status : uint8_t {
    ok,
    emptyParams,
    invalidParam
};

struct ExponentialMovingAverageState {
    double lastEma{0.0};
    double period;
    double alpha;
    double oma;
};

struct SimpleMovingAverageState {
    double alpha{0.0};
    size_t period{0};
    double rollingSum{0.0};
    bool initialized{false};
    double lastSma{0.0};
    std::vector<double> priceBuf;
};

struct WeightedMovingAverageState {
    size_t period{0};
    double denominator{0.0};
    double rollingSum{0.0};
    double rollingWeightedSum{0.0};
    bool initialized{false};
    double lastWma{0.0};
    std::vector<double> priceBuf;
};

struct VolumeWeightedMovingAverageState {
    size_t period{0};
    bool initialized{false};
    double rollingNumerator{0.0};
    double rollingDenominator{0.0};
    double lastCalculation{0.0};
    std::vector<double> priceBuf;
    std::vector<double> volumeBuf;
};

struct HullMovingAverageState {
    uint16_t p1{0};
    uint16_t p2{0};
    uint16_t period{0};
    double lastHull{0.0};
    bool initialized{false};
    WeightedMovingAverageState w1;
    WeightedMovingAverageState w2;
    WeightedMovingAverageState w3;
};

struct DoubleExponentialMovingAverageState {
    size_t period{0};
    bool initialized{false};
    double lastDema{0.0};
    ExponentialMovingAverageState ema1;
    ExponentialMovingAverageState ema2;
};

struct TripleExponentialMovingAverageState {
    size_t period{0};
    bool initialized{false};
    double lastTema{0.0};
    ExponentialMovingAverageState ema1;
    ExponentialMovingAverageState ema2;
    ExponentialMovingAverageState ema3;
};

struct McGinleyDynamicMovingAverageState {
    uint16_t period{0};
    double lastMd{0.0};
    bool initialized{false};
};

struct FractalAdaptiveMovingAverageState {
    bool initialized{false};
    size_t period{0};
    double eulerNumber{-4.6};
    double halfPeriod{0.0};
    double logTwo{0.0};
    double highBuf1Max{0.0};
    double highBuf2Max{0.0};
    double lowBuf1min{0.0};
    double lowBuf2min{0.0};
    double lastFrama{0.0};
    std::vector<double> highBuf1;
    std::vector<double> highBuf2;
    std::vector<double> lowBuf1;
    std::vector<double> lowBuf2;
};

struct GeneralizedDoubleExponentialMovingAverageState {
    double period{0.0};
    double emaPeriod{0.0};
    double onePlusPeriod{0.0};
    double lastGd{0.0};
    ExponentialMovingAverageState ema1;
    ExponentialMovingAverageState ema2;
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
        ExponentialMovingAverage(uint16_t period);
        ExponentialMovingAverage(ExponentialMovingAverageState prevCalculation);
        // ExponentialMovingAverage(ExponentialMovingAverageState state);

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

        ExponentialMovingAverageState getState();
        
    };

    /// Stateful Simple Moving Average (SMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class SimpleMovingAverage {    
    private:
        double alpha;
        size_t period;
        double rollingSum{0.0};
        bool initalized{false};
        double lastSma{0.0};
        helpers::RingBuffer<double> priceBuf;
    public:
        /// Creates an SMA indicator instance.
        /// @param period Number of samples used in the SMA window.
        SimpleMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
        SimpleMovingAverage(SimpleMovingAverageState prevCalculation);

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

        SimpleMovingAverageState getState();
    };


    class WeightedMovingAverage {
        private: 
            size_t period;
            double denominator;

            
            double rollingSum{0.0};
            double rollingWeightedSum{0.0};
            
            bool initialized{false};
            double lastWma{0.0};
            helpers::RingBuffer<double> priceBuf;

        public:
            /// Creates a WMA indicator instance.
            /// @param period Number of samples used in the WMA window.
            /// @param prevCalc Optional warm-start buffer containing the latest `period`
            /// prices ordered from past to present (oldest to newest).
            WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
            WeightedMovingAverage(WeightedMovingAverageState prevCalculation);

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

            WeightedMovingAverageState getState();

    };

    class VolumeWeightedMovingAverage {
        private: 
            size_t period;
            bool initialized;
            double rollingNumerator;
            double rollingDenominator;
            double lastCalculation;
            helpers::RingBuffer<double> priceBuf;
            helpers::RingBuffer<double> volumeBuf;
            
        public:
            VolumeWeightedMovingAverage(uint16_t period, std::vector<double> prevPrices = {}, std::vector<double> prevVolume = {});
            VolumeWeightedMovingAverage(VolumeWeightedMovingAverageState prevCalculation);
            status compute(std::span<const double> prices, std::span<const double> volume, std::vector<double>& output);
            double update(double price, double volume);
            double latest();
            VolumeWeightedMovingAverageState getState();
    };



    class HullMovingAverage {
        uint16_t p1;
        uint16_t p2;
        uint16_t period;        
        double lastHull = 0.0;
        bool initialized = false;

        WeightedMovingAverage w1;
        WeightedMovingAverage w2;
        WeightedMovingAverage w3;
    public:
        /// Creates an HMA indicator instance.
        /// @param period Base lookback period used by the HMA.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        HullMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
        HullMovingAverage(HullMovingAverageState prevCalculation);

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

        HullMovingAverageState getState();
    };

    /// Stateful Double Exponential Moving Average (DEMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class DoubleExponentialMovingAverage {
    private:
        size_t period;
        bool initialized{false};
        double lastDema{0.0};

        ExponentialMovingAverage ema1;
        ExponentialMovingAverage ema2;
    public:
        /// Creates a DEMA indicator instance.
        /// @param period Lookback period used by the EMA cascade.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        DoubleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
        DoubleExponentialMovingAverage(DoubleExponentialMovingAverageState prevCalculation);

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

        DoubleExponentialMovingAverageState getState();
    };

    /// Stateful Triple Exponential Moving Average (TEMA) indicator.
    /// Supports both batch computation and single-tick updates.
    class TripleExponentialMovingAverage {
    private:
        size_t period;    
        bool initialized{false};
        double lastTema{0.0};

        ExponentialMovingAverage ema1;
        ExponentialMovingAverage ema2;
        ExponentialMovingAverage ema3;

    public:
        /// Creates a TEMA indicator instance.
        /// @param period Lookback period used by the EMA cascade.
        /// @param prevCalc Optional warm-start buffer containing the latest `period`
        /// prices ordered from past to present (oldest to newest).
        TripleExponentialMovingAverage(uint16_t period, std::vector<double> prevCalc = {});
        TripleExponentialMovingAverage(TripleExponentialMovingAverageState prevCalculation);

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

        TripleExponentialMovingAverageState getState();
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
        McGinleyDynamicMovingAverage(McGinleyDynamicMovingAverageState prevCalculation);

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

        McGinleyDynamicMovingAverageState getState();
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
        FractalAdaptiveMovingAverage(FractalAdaptiveMovingAverageState prevCalculation);
        status compute(std::span<const double> close, std::span<const double> low, std::span<const double> high, std::vector<double>& output);
        double latest();
        double update(double close, double low, double high);
        FractalAdaptiveMovingAverageState getState();
    };

    class GeneralizedDoubleExponentialMovingAverage  {
    private:
        double period;
        double emaPeriod;
        double onePlusPeriod;
        double lastGd{0.0};
        ExponentialMovingAverage emaBuf1;
        ExponentialMovingAverage emaBuf2;

    public:
        GeneralizedDoubleExponentialMovingAverage(double period, uint16_t emaPeriod);
        GeneralizedDoubleExponentialMovingAverage(GeneralizedDoubleExponentialMovingAverageState prevCalculation);
        status compute(std::span<const double> price, std::vector<double>& output);
        double latest();
        double update(double price);
        GeneralizedDoubleExponentialMovingAverageState getState();
    };  

    class KaufmanAdaptiveMovingAverage  {
        private:
    }; 


 } // namespace tama



