#include <tama/tama.hpp>
#include <helpers/helpers.hpp>
#include <algorithm>
#include <cmath>
#include <span>
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

tama::SimpleMovingAverage::SimpleMovingAverage(uint16_t period, std::vector<double> prevCalc)
    : alpha(0.0),
      period(static_cast<size_t>(period)),
      rollingSum(0.0),
      initalized(false),
    lastSma(0.0),
    priceBuf(period > 0 ? period : 1) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    this->alpha = 1.0 / static_cast<double>(this->period);

    if (!prevCalc.empty()) {
        if (prevCalc.size() != this->period) {
            throw std::invalid_argument("prevCalc buffer doesn't match period");
        }

        this->priceBuf.insert(prevCalc);
        this->rollingSum = helpers::simdSum(prevCalc);
        this->lastSma = this->alpha * this->rollingSum;
        this->initalized = true;
    }
}

tama::SimpleMovingAverage::SimpleMovingAverage(SimpleMovingAverageState prevCalculation)
    : alpha(prevCalculation.alpha),
      period(prevCalculation.period),
      rollingSum(prevCalculation.rollingSum),
      initalized(prevCalculation.initialized),
      lastSma(prevCalculation.lastSma),
      priceBuf(prevCalculation.period > 0 ? prevCalculation.period : 1) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    const double expectedAlpha = 1.0 / static_cast<double>(this->period);
    if (std::abs(this->alpha - expectedAlpha) > 1e-12) {
        throw std::invalid_argument("invalid alpha for provided period");
    }

    if (!prevCalculation.priceBuf.empty()) {
        if (prevCalculation.priceBuf.size() != this->period) {
            throw std::invalid_argument("priceBuf size doesn't match period");
        }
        this->priceBuf.insert(prevCalculation.priceBuf);
    }

    if (this->initalized && this->priceBuf.len() != this->period) {
        throw std::invalid_argument("initialized SMA state requires a full buffer");
    }
}

double tama::SimpleMovingAverage::latest() {
    return this->lastSma;
}

SimpleMovingAverageState tama::SimpleMovingAverage::getState() {
    return {
        .alpha = this->alpha,
        .period = this->period,
        .rollingSum = this->rollingSum,
        .initialized = this->initalized,
        .lastSma = this->lastSma,
        .priceBuf = ring_to_vector(this->priceBuf)
    };
}

status tama::SimpleMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty()) {
        return status::emptyParams;
    }
    const size_t pricesLen = prices.size();

    if (this->period >= pricesLen) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }
    std::fill(output.begin(), output.begin() + this->period - 1, 0.0);

    double sum = helpers::simdSum(prices.subspan(0, this->period));
    output[this->period - 1] = this->alpha * sum;
    
    for (size_t t = this->period; t < pricesLen; t++) {
        sum += prices[t] - prices[t - this->period];
        output[t] = this->alpha * sum;
    }

    std::span<const double> tail = prices.subspan(pricesLen - this->period, this->period);
    this->priceBuf.insert(std::vector<double>(tail.begin(), tail.end()));
    this->rollingSum = sum;
    this->initalized = true;
    this->lastSma = output.back();

    return status::ok;
}

double tama::SimpleMovingAverage::update(double price) {
    if (!this->initalized) {
        throw std::runtime_error("sma not initialized");
    }

    this->rollingSum -= this->priceBuf.head();
    this->rollingSum += price;
    this->priceBuf.insert(price);

    double sma = this->alpha * this->rollingSum;
    this->lastSma = sma;
    return sma;
}
