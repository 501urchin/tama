#include <vector>
#include <tama/tama.hpp>


using namespace std;

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc) {
    this->denominator = static_cast<double>(period) * static_cast<double>((period + 1)) / 2;
    this->period = period;
    this->head = 0;
    this->rollingSum = 0;
    this->rollingWeightedSum = 0;

    if (this->period > 0 && prevCalc.size() >= this->period) {
        const size_t offset = prevCalc.size() - this->period;
        this->priceBuf.assign(prevCalc.begin() + static_cast<std::ptrdiff_t>(offset), prevCalc.end());

        double weightedSum = 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < this->period; i++) {
            sum += this->priceBuf[i];
            weightedSum += this->priceBuf[i] * static_cast<double>(i + 1);
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
    this->priceBuf.assign(prices.begin() + static_cast<std::ptrdiff_t>(offset), prices.end());
    this->head = 0;
    this->rollingSum = sSum;
    this->rollingWeightedSum = weightedSum;
    this->lastWma = output.back();
    this->initalized = true;

    return status::ok;
}

double tama::WeightedMovingAverage::update(double price) {
    if (!this->initalized || this->priceBuf.size() < this->period) {
        return 0;
    }

    const double oldSum = this->rollingSum;
    const double dropped = this->priceBuf[this->head];

    this->rollingWeightedSum = this->rollingWeightedSum - oldSum + (price * static_cast<double>(this->period));
    this->rollingSum = oldSum - dropped + price;

    this->priceBuf[this->head] = price;
    this->head = (this->head + 1) % this->period;

    this->lastWma = this->rollingWeightedSum / this->denominator;


    return this->lastWma;
}