#include <tama/tama.hpp>
#include <tama/helpers.hpp>
#include <algorithm>
#include <span>


using namespace tama::helpers;

SimpleMovingAverage::SimpleMovingAverage(uint16_t period) {
    this->period = period;
    this->alpha = 1.0 / static_cast<double>(period);
    this->priceBuf = std::vector<double>(period);
}

double SimpleMovingAverage::latest() {
    return this->lastSma;
}

status SimpleMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty() ) {
        return status::emptyParams;
    }
    size_t pricesLen = prices.size();

    if (this->period >= pricesLen || this->period == 0 ) {
        return status::invalidParam;
    }

    if (output.size() < pricesLen) {
        output.resize(pricesLen);
    }
    std::fill(output.begin(), output.begin() + this->period-1, 0);


    double sum = simdSum(prices.subspan(0, this->period));
    output[this->period-1] = alpha * sum;
    
    for (size_t t = this->period; t < pricesLen; t++ ) {
        sum += prices[t] - prices[t-this->period];
        output[t] = alpha * sum;
    }

    std::span<const double> tail = prices.subspan(pricesLen - this->period, this->period);
    this->priceBuf = std::vector<double>(tail.begin(), tail.end());
    this->lastSma = output.back();
    this->initalized = true;
    return status::ok;
}

double SimpleMovingAverage::update(double price) {
    if (!this->initalized) {
        return 0;
    }

    this->priceBuf.erase(this->priceBuf.begin());
    this->priceBuf.push_back(price);

    double sma = this->alpha * simdSum(this->priceBuf);
    this->lastSma = sma;
    return sma;
}
