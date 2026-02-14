#include <vector>
#include <cmath>
#include <tama/tama.hpp>

using namespace std;

ExponentialMovingAverage::ExponentialMovingAverage(uint16_t period, double prevCalculation) {
    this->period = static_cast<double>(period);
    if (period == 0) {
        this->alpha = 0.0;
        this->oma = 0.0;
        this->lastEma = 0.0;
        return;
    }

    this->alpha = 2.0 / (this->period + 1.0);
    this->oma = 1.0 - alpha;
    if (!std::isnan(prevCalculation)) {
        this->lastEma = prevCalculation;
        this->initalized = true;
    }
}



status ExponentialMovingAverage::compute(std::span<const double> prices, std::vector<double>& output){
    if (prices.empty() ) {
        return status::emptyParams;
    }

    if (this->period == 0) {
        return status::invalidParam;
    }
    size_t pricesLen = prices.size();

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }
    output[0] = prices[0];

    for (size_t t = 1; t < pricesLen; t++) {
        output[t] = this->alpha * prices[t] + this->oma * output[t-1];
    }

    this->initalized = true;
    this->lastEma = output.back();

    return status::ok;
}

double ExponentialMovingAverage::update(double price){
    if (!this->initalized) {
        return 0.0;
    }

    double ema = this->alpha * price + this->oma * this->lastEma;
    this->lastEma = ema;
    return ema;
}

double ExponentialMovingAverage::latest(){
    return this->lastEma;
}