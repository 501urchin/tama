#include <vector>
#include <stdexcept>
#include <tama/tama.hpp>

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc)
    : priceBuf(period > 0 ? period : 1) {
    if (period == 0) {
        throw std::invalid_argument("invalid period");
    }
    this->denominator = static_cast<double>(period) * static_cast<double>((period + 1)) / 2;
    this->period = period;
    this->rollingSum = 0;
    this->rollingWeightedSum = 0;

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
        this->initalized = true;
    }
}

double tama::WeightedMovingAverage::latest() {
    return this->lastWma;
}

status tama::WeightedMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    size_t pricesLen = prices.size();

    if (pricesLen == 0) {
        return status::emptyParams;
    }

    if (this->period == 0 || this->period > pricesLen) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }

    double sSum = 0;
    double weightedSum = 0;


    for (size_t i = 0; i < this->period; i++) {
        sSum += prices[i];
        weightedSum += prices[i] * static_cast<double>(i + 1);
    }
    output[this->period-1] = weightedSum / this->denominator;

    for (size_t t = this->period; t < pricesLen; t++) {
        weightedSum -= sSum;
        sSum -= prices[t-this->period];
        sSum += prices[t];
        weightedSum += prices[t] * this->period;

        output[t] = weightedSum / this->denominator;
    }

    const size_t offset = pricesLen - this->period;
    this->priceBuf = helpers::RingBuffer<double>(this->period);
    std::vector<double> tail(prices.begin() + static_cast<std::ptrdiff_t>(offset), prices.end());
    this->priceBuf.insert(tail);
    this->rollingSum = sSum;
    this->rollingWeightedSum = weightedSum;
    this->lastWma = output.back();
    this->initalized = true;

    return status::ok;
}

double tama::WeightedMovingAverage::update(double price) {
    if (!this->initalized || static_cast<size_t>(this->priceBuf.len()) < this->period) {
        return 0;
    }

    const double oldSum = this->rollingSum;

    this->rollingWeightedSum = this->rollingWeightedSum - oldSum + (price * static_cast<double>(this->period));
    this->rollingSum = oldSum - this->priceBuf.head() + price;

    this->priceBuf.insert(price);

    this->lastWma = this->rollingWeightedSum / this->denominator;


    return this->lastWma;
}