#include <vector>
#include <cmath>
#include <stdexcept>
#include <stdexcept>
#include <tama/tama.hpp>

tama::ExponentialMovingAverage::ExponentialMovingAverage(uint16_t period, double prevCalculation)
    : lastEma(0.0),
      period(static_cast<double>(period)),
      alpha(0.0),
      oma(0.0),
      initalized(false) {
    if (period == 0) {
        throw std::invalid_argument("invalid period");
    }

    this->alpha = 2.0 / (this->period + 1.0);
    this->oma = 1.0 - this->alpha;
    if (!std::isnan(prevCalculation) && prevCalculation != 0.0) {
        this->lastEma = prevCalculation;
        this->initalized = true;
    }
}

status tama::ExponentialMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty()) {
        return status::emptyParams;
    }

    const size_t pricesLen = prices.size();

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }
    output[0] = prices[0];

    for (size_t t = 1; t < pricesLen; t++) {
        output[t] = this->alpha * prices[t] + this->oma * output[t - 1];
    }

    this->initalized = true;
    this->lastEma = output.back();

    return status::ok;
}

double tama::ExponentialMovingAverage::update(double price) {
    if (!this->initalized) {
        throw std::runtime_error("ema not initialized");
    }

    double ema = this->alpha * price + this->oma * this->lastEma;
    this->lastEma = ema;
    return ema;
}

double tama::ExponentialMovingAverage::latest() {
    return this->lastEma;
}