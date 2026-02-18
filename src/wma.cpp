#include <vector>
#include <stdexcept>
#include <tama/tama.hpp>

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc)
    : period(static_cast<size_t>(period)),
      denominator(static_cast<double>(period) * static_cast<double>(period + 1) / 2.0),
      priceBuf(period > 0 ? period : 1),
      rollingSum(0.0),
      rollingWeightedSum(0.0),
      initialized(false),
      lastWma(0.0) {
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

double tama::WeightedMovingAverage::latest() {
    return this->lastWma;
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
        return 0.0;
    }

    const double oldSum = this->rollingSum;

    this->rollingWeightedSum = this->rollingWeightedSum - oldSum + (price * this->period);
    this->rollingSum = oldSum - this->priceBuf.head() + price;

    this->priceBuf.insert(price);

    this->lastWma = this->rollingWeightedSum / this->denominator;


    return this->lastWma;
}