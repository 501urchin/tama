#include <tama/tama.hpp>
#include <cmath>
#include <stdexcept>

tama::McGinleyDynamicMovingAverage::McGinleyDynamicMovingAverage(uint16_t period, double prevCalculation)
    : period(period),
      lastMd(0.0),
      initialized(false) {
    if (period == 0) {
        throw std::invalid_argument("invalid period");
    }

    if (!std::isnan(prevCalculation) && prevCalculation != 0.0) {
        this->lastMd = prevCalculation;
        this->initialized = true;
    }
}

status tama::McGinleyDynamicMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty()) {
        return status::emptyParams;
    }

    const size_t pricesLen = prices.size();
    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }

    output[0] = prices[0];

    for (size_t t = 1; t < pricesLen; t++) {
        const double pt = prices[t];
        const double mt = output[t - 1];

        const double numerator = pt - mt;
        const double denominator = static_cast<double>(this->period) * std::pow(pt / mt, 4.0);

        output[t] = numerator / denominator + mt;
    }

    this->lastMd = output.back();
    this->initialized = true;

    return status::ok;
}

double tama::McGinleyDynamicMovingAverage::update(double price) {
    if (!this->initialized) {
        throw std::runtime_error("md not initialized");
    }

    const double mt = this->lastMd;
    const double numerator = price - mt;
    const double denominator = static_cast<double>(this->period) * std::pow(price / mt, 4.0);

    this->lastMd = numerator / denominator + mt;
    return this->lastMd;
}

double tama::McGinleyDynamicMovingAverage::latest() {
    return this->lastMd;
}