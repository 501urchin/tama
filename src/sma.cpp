#include <tama/tama.hpp>
#include <tama/helpers.hpp>
#include <algorithm>
#include <span>
#include <stdexcept>



tama::SimpleMovingAverage::SimpleMovingAverage(uint16_t period, std::vector<double> prevCalc): period(period),priceBuf(period > 0 ? period : 1),  rollingSum(0.0), lastSma(0.0) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    this->alpha = 1.0 / static_cast<double>(this->period);

    if (!prevCalc.empty()) {
        if (prevCalc.size() != period) {
            throw std::invalid_argument("prevCalc buffer doesn't match period");
        }
        
        this->priceBuf.insert(prevCalc);
        this->rollingSum = helpers::simdSum(prevCalc);
        this->lastSma = this->alpha * this->rollingSum;
        this->initalized = true;
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
    this->rollingSum = sum;
    this->priceBuf.insert(std::vector<double>(tail.begin(), tail.end()));
    this->initalized = true;
    this->lastSma = output.back();

    return status::ok;
}

double tama::SimpleMovingAverage::update(double price) {
    if (!this->initalized) {
        return 0;
    }

    this->rollingSum -= this->priceBuf.head();
    this->rollingSum +=  price;
    this->priceBuf.insert(price);

    double sma = this->alpha * this->rollingSum;
    this->lastSma = sma;
    return sma;
}
