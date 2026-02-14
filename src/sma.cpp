#include <tama/tama.hpp>
#include <tama/helpers.hpp>
#include <algorithm>
#include <span>



tama::SimpleMovingAverage::SimpleMovingAverage(uint16_t period, std::vector<double> prevCalc) {
    this->period = period;
    this->lastSma = 0.0;

    if (this->period == 0) {
        this->alpha = 0.0;
        return;
    }

    this->alpha = 1.0 / static_cast<double>(this->period);

    if (prevCalc.size() >= this->period) {
        const size_t offset = prevCalc.size() - this->period;
        this->priceBuf.assign(prevCalc.begin() + static_cast<std::ptrdiff_t>(offset), prevCalc.end());
        this->lastSma = this->alpha * helpers::simdSum(this->priceBuf);
        this->initalized = true;
    } else {
        this->priceBuf = std::vector<double>(this->period);
    }
}

double tama::SimpleMovingAverage::latest() {
    return this->lastSma;
}

status tama::SimpleMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
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


    double sum = helpers::simdSum(prices.subspan(0, this->period));
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

double tama::SimpleMovingAverage::update(double price) {
    if (!this->initalized) {
        return 0;
    }

    this->priceBuf.erase(this->priceBuf.begin());
    this->priceBuf.push_back(price);

    double sma = this->alpha * helpers::simdSum(this->priceBuf);
    this->lastSma = sma;
    return sma;
}
