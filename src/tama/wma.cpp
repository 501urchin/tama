#include <vector>
#include <cmath>
#include <stdexcept>
#include <tama/tama.hpp>

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

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc)
    : period(static_cast<size_t>(period)),
      denominator(static_cast<double>(period) * static_cast<double>(period + 1) / 2.0),
      rollingSum(0.0),
      rollingWeightedSum(0.0),
      initialized(false),
    lastWma(0.0),
    priceBuf(period > 0 ? period : 1) {
    if (period == 0) {
        throw std::invalid_argument("invalid period");
    }

    if (!prevCalc.empty()) {
        if (prevCalc.size() != this->period) {
            throw std::invalid_argument("prevCalc buffer doesn't match period");
        }
        this->priceBuf.insert(prevCalc);

        double weightedSum = 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < this->period; i++) {
            sum += this->priceBuf[i];
            weightedSum += this->priceBuf[static_cast<int>(i)] * static_cast<double>(i + 1);
        }

        this->rollingSum = sum;
        this->rollingWeightedSum = weightedSum;
        this->lastWma = weightedSum / this->denominator;
        this->initialized = true;
    }
}

tama::WeightedMovingAverage::WeightedMovingAverage(WeightedMovingAverageState prevCalculation)
    : period(prevCalculation.period),
      denominator(prevCalculation.denominator),
      rollingSum(prevCalculation.rollingSum),
      rollingWeightedSum(prevCalculation.rollingWeightedSum),
      initialized(prevCalculation.initialized),
    lastWma(prevCalculation.lastWma),
    priceBuf(prevCalculation.period > 0 ? prevCalculation.period : 1) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    const double expectedDenominator = static_cast<double>(this->period) * static_cast<double>(this->period + 1) / 2.0;
    if (std::abs(this->denominator - expectedDenominator) > 1e-12) {
        throw std::invalid_argument("invalid denominator for provided period");
    }

    if (!prevCalculation.priceBuf.empty()) {
        if (prevCalculation.priceBuf.size() != this->period) {
            throw std::invalid_argument("priceBuf size doesn't match period");
        }
        this->priceBuf.insert(prevCalculation.priceBuf);
    }

    if (this->initialized && this->priceBuf.len() != this->period) {
        throw std::invalid_argument("initialized WMA state requires a full buffer");
    }
}

double tama::WeightedMovingAverage::latest() {
    return this->lastWma;
}

WeightedMovingAverageState tama::WeightedMovingAverage::getState() {
    return {
        .period = this->period,
        .denominator = this->denominator,
        .rollingSum = this->rollingSum,
        .rollingWeightedSum = this->rollingWeightedSum,
        .initialized = this->initialized,
        .lastWma = this->lastWma,
        .priceBuf = ring_to_vector(this->priceBuf)
    };
}

status tama::WeightedMovingAverage::compute(
    std::span<const double> prices,
    std::vector<double>& output) {
    const size_t n = prices.size();

    if (n == 0) {
        return status::emptyParams;
    }
    if (this->period > n) {
        return status::invalidParam;
    }

    output.resize(n);

    double sSum = 0.0;
    double weightedSum = 0.0;


    for (size_t i = 0; i < this->period; ++i) {
        const double value = prices[i];
        sSum += value;
        weightedSum += value * static_cast<double>(i + 1);
    }

    output[this->period - 1] = weightedSum / this->denominator;


    for (size_t t = this->period; t < n; ++t) {
        weightedSum -= sSum;
        sSum -= prices[t - this->period];

        const double value = prices[t];
        sSum += value;
        weightedSum += value * static_cast<double>(this->period);

        output[t] = weightedSum / this->denominator;
    }

    const size_t offset = n - this->period;

    this->priceBuf = helpers::RingBuffer<double>(this->period);
    this->priceBuf.insert(std::vector<double>(prices.begin() + offset, prices.end()));

    this->rollingSum = sSum;
    this->rollingWeightedSum = weightedSum;
    this->lastWma = output.back();
    this->initialized = true;

    return status::ok;
}


double tama::WeightedMovingAverage::update(double price) {
    if (!this->initialized) {
        throw std::runtime_error("wma not initialized");
    }

    const double oldSum = this->rollingSum;

    this->rollingWeightedSum = this->rollingWeightedSum - oldSum + (price * this->period);
    this->rollingSum = oldSum - this->priceBuf.head() + price;

    this->priceBuf.insert(price);

    this->lastWma = this->rollingWeightedSum / this->denominator;


    return this->lastWma;
}