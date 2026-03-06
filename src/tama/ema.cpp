#include <vector>
#include <cmath>
#include <stdexcept>
#include <stdexcept>
#include <tama/tama.hpp>


tama::ExponentialMovingAverage::ExponentialMovingAverage(uint16_t period)
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
}


tama::ExponentialMovingAverage::ExponentialMovingAverage(ExponentialMovingAverageState prevCalculation)
    :   lastEma(prevCalculation.lastEma),
        period(prevCalculation.period),
        alpha(prevCalculation.alpha),
        oma(prevCalculation.oma),
        initalized(true) {


    if (period <= 0) {
        throw std::invalid_argument("Invalid period: must be > 0");
    }

    if (alpha <= 0.0 || alpha > 1.0) {
        throw std::invalid_argument("Invalid alpha: must be in (0, 1]");
    }

    if (oma < 0.0) {
        throw std::invalid_argument("Invalid OMA: cannot be negative");
    }

    if (std::isnan(lastEma)) {
        throw std::invalid_argument("Invalid lastEma: cannot be NaN");
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


ExponentialMovingAverageState tama::ExponentialMovingAverage::getState() {
    return {
        .lastEma = this->lastEma,
        .period = this->period,
        .alpha = this->alpha,
        .oma = this->oma
    };
}