#include <vector>
#include <tama/tama.hpp>

using namespace std;

tama::WeightedMovingAverage::WeightedMovingAverage(uint16_t period, std::vector<double> prevCalc) {
    this->denominator = static_cast<double>(period) * static_cast<double>((period + 1)) / 2;
    this->period = period;

    if (this->period > 0 && prevCalc.size() >= this->period) {
        const size_t offset = prevCalc.size() - this->period;
        this->priceBuf.assign(prevCalc.begin() + static_cast<std::ptrdiff_t>(offset), prevCalc.end());

        double weightedSum = 0.0;
        for (size_t i = 0; i < this->period; i++) {
            weightedSum += this->priceBuf[i] * (i + 1);
        }

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

    if (this->period == 0 || this->period >= pricesLen) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }

    double sSum = 0;
    double weightedSum = 0;


    for (size_t i = 0; i < this->period; i++) {
        sSum += prices[i];
        weightedSum += prices[i] * (i+1);
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
    this->lastWma = output.back();
    this->initalized = true;

    return status::ok;
}

double tama::WeightedMovingAverage::update(double price) {
    if (!this->initalized || this->priceBuf.size() < this->period) {
        return 0;
    }

    this->priceBuf.erase(this->priceBuf.begin());
    this->priceBuf.push_back(price);


    double weightedSum = 0.0;
    for (size_t i = 0; i < this->period; i++) {
        weightedSum += this->priceBuf[i] * (i + 1);
    }

    this->lastWma = weightedSum / this->denominator;


    return this->lastWma;
}