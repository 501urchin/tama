#include <vector>
#include <tama/tama.hpp>
#include <stdexcept>

namespace {
std::vector<double> ring_to_vector(const helpers::RingBuffer<double>& buffer) {
    std::vector<double> values;
    values.reserve(buffer.len());
    for (size_t i = 0; i < buffer.len(); ++i) {
        values.push_back(buffer[i]);
    }
    return values;
}
}

tama::VolumeWeightedMovingAverage::VolumeWeightedMovingAverage(uint16_t period, std::vector<double> prevPrices, std::vector<double> prevVolume)
    : period(static_cast<size_t>(period)),
    initialized(false),
      rollingNumerator(0.0),
      rollingDenominator(0.0),
    lastCalculation(0.0),
    priceBuf(period > 0 ? period : 1),
    volumeBuf(period > 0 ? period : 1) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    if (prevPrices.empty() != prevVolume.empty()) {
        throw std::invalid_argument("prevPrices and prevVolume must both be empty or both be provided");
    }

    if (!prevPrices.empty()) {
        if (prevPrices.size() != this->period) {
            throw std::invalid_argument("prevPrices buffer doesn't match period");
        }

        if (prevVolume.size() != this->period) {
            throw std::invalid_argument("prevVolume buffer doesn't match period");
        }

        if (prevVolume.size() != prevPrices.size()) {
            throw std::invalid_argument("prevVolume buffer doesn't match prevPrices buffer");
        }
        
        this->priceBuf.insert(prevPrices);
        this->volumeBuf.insert(prevVolume);

        for (size_t i = 0; i < this->period; i++) {
            this->rollingNumerator += prevPrices[i] * prevVolume[i];
            this->rollingDenominator += prevVolume[i];
        }

        this->lastCalculation = this->rollingNumerator / this->rollingDenominator;
        this->initialized = true;
    }   
}

tama::VolumeWeightedMovingAverage::VolumeWeightedMovingAverage(VolumeWeightedMovingAverageState prevCalculation)
    : period(prevCalculation.period),
      initialized(prevCalculation.initialized),
      rollingNumerator(prevCalculation.rollingNumerator),
      rollingDenominator(prevCalculation.rollingDenominator),
      lastCalculation(prevCalculation.lastCalculation),
      priceBuf(prevCalculation.period > 0 ? prevCalculation.period : 1),
      volumeBuf(prevCalculation.period > 0 ? prevCalculation.period : 1) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    if (prevCalculation.priceBuf.size() != prevCalculation.volumeBuf.size()) {
        throw std::invalid_argument("priceBuf and volumeBuf must match in size");
    }

    if (!prevCalculation.priceBuf.empty()) {
        if (prevCalculation.priceBuf.size() != this->period) {
            throw std::invalid_argument("priceBuf size doesn't match period");
        }
        this->priceBuf.insert(prevCalculation.priceBuf);
        this->volumeBuf.insert(prevCalculation.volumeBuf);
    }

    if (this->initialized && this->priceBuf.len() != this->period) {
        throw std::invalid_argument("initialized VWMA state requires a full buffer");
    }
}

double tama::VolumeWeightedMovingAverage::latest() {
    return this->lastCalculation;
}

VolumeWeightedMovingAverageState tama::VolumeWeightedMovingAverage::getState() {
    return {
        .period = this->period,
        .initialized = this->initialized,
        .rollingNumerator = this->rollingNumerator,
        .rollingDenominator = this->rollingDenominator,
        .lastCalculation = this->lastCalculation,
        .priceBuf = ring_to_vector(this->priceBuf),
        .volumeBuf = ring_to_vector(this->volumeBuf)
    };
}

status tama::VolumeWeightedMovingAverage::compute(std::span<const double> prices, std::span<const double> volume, std::vector<double>& output) {
    const size_t pricesLen = prices.size();
    const size_t volumeLen = volume.size();

    if (pricesLen == 0 || volumeLen == 0) {
        return status::emptyParams;
    }

    if (pricesLen != volumeLen || this->period >= pricesLen) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }

    double numeratorSum = 0.0;
    double denominatorSum = 0.0;

    // simd
    for (size_t i = 0; i < this->period; i++) {
        numeratorSum += prices[i] * volume[i];
        denominatorSum += volume[i];
    }
    output[this->period - 1] = numeratorSum / denominatorSum;

    for (size_t t = this->period; t < pricesLen; t++) {
        numeratorSum -= prices[t - this->period] * volume[t - this->period];
        numeratorSum += prices[t] * volume[t];

        denominatorSum -= volume[t - this->period];
        denominatorSum += volume[t];

        output[t] = numeratorSum / denominatorSum;
    }

    std::span<const double> pricesTail = prices.subspan(pricesLen - this->period, this->period);
    this->priceBuf.insert(std::vector<double>(pricesTail.begin(), pricesTail.end()));

    std::span<const double> volumeTail = volume.subspan(volumeLen - this->period, this->period);
    this->volumeBuf.insert(std::vector<double>(volumeTail.begin(), volumeTail.end()));

    this->lastCalculation = output.back();
    this->rollingNumerator = numeratorSum;
    this->rollingDenominator = denominatorSum;
    this->initialized = true;

    return status::ok;    
}

double tama::VolumeWeightedMovingAverage::update(double price, double volume) {
    if (!this->initialized) {
        throw std::runtime_error("vwma not initialized");
    }

    this->rollingNumerator -= this->priceBuf.head() * this->volumeBuf.head();
    this->rollingNumerator += price * volume;

    this->rollingDenominator -= this->volumeBuf.head();
    this->rollingDenominator += volume;

    this->priceBuf.insert(price);
    this->volumeBuf.insert(volume);

    double calc = this->rollingNumerator / this->rollingDenominator;

    this->lastCalculation = calc;

    return calc;
}