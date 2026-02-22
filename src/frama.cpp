#include "tama/tama.hpp"
#include <cmath>
#include <algorithm>

#include <iostream>
#include <vector>



namespace tama {
    FractalAdaptiveMovingAverage::FractalAdaptiveMovingAverage(uint16_t period, double eulerNumber): period(period), eulerNumber(eulerNumber),  halfPeriod(period/2), logTwo(log(2)), highBuf(period), lowBuf(period), closeBuf(period) {}

    double FractalAdaptiveMovingAverage::latest() {
        return this->lastFrama;
    }

    status FractalAdaptiveMovingAverage::compute(std::span<const double> close, std::span<const double> low, std::span<const double> high, std::vector<double>& output) {
        size_t lowLen = low.size();
        size_t highLen = high.size();
        size_t closeLen = close.size();


        if (low.empty() || high.empty() || close.empty())
            return status::emptyParams;

        if (lowLen != highLen || closeLen != lowLen)
            return status::invalidParam;

        if (closeLen < this->period)
            return status::invalidParam;

        if (output.size() < closeLen) {
            output.resize(closeLen);
        }
        std::fill(output.begin(), output.begin() + this->period - 1, 0.0);

        output[this->period-1] = close[0];

        for (size_t i = 0; i < this->period; i ++) {
            output[i] = close[i];
        }

        // edge case: what if period is uneven? how wil window splitting be handled
        for (size_t i = this->period; i <  closeLen; i++) {
            std::span<const double> fullWindowHigh =  high.subspan(i - this->period, this->period);
            std::span<const double> windowOneHigh =  high.subspan(i - this->period, this->halfPeriod);
            std::span<const double> windowTwoHigh = high.subspan(i - this->halfPeriod, this->halfPeriod);
            
            std::span<const double> fullWindowLow =  low.subspan(i - this->period, this->period);
            std::span<const double> windowOneLow =  low.subspan(i - this->period, this->halfPeriod);
            std::span<const double> windowTwoLow = low.subspan(i - this->halfPeriod, this->halfPeriod);

            double l1 = (*std::max_element(windowOneHigh.begin(), windowOneHigh.end()) - *std::min_element(windowOneLow.begin(), windowOneLow.end())) / this->halfPeriod;
            double l2 = (*std::max_element(windowTwoHigh.begin(), windowTwoHigh.end()) - *std::min_element(windowTwoLow.begin(), windowTwoLow.end())) / this->halfPeriod;
            double l3 = (*std::max_element(fullWindowHigh.begin(), fullWindowHigh.end()) - *std::min_element(fullWindowLow.begin(), fullWindowLow.end())) / this->period;
            double D = log((l1 + l2) / l3) / this->logTwo;
            if (D <= 0) {
                D = 1;
            }
            double alpha = exp(this->eulerNumber * (D - 1));

            output[i] = alpha * close[i] + (1-alpha) * output[i-1];
        }
        
        return status::ok;
    }
}