#include "tama/tama.hpp"
#include <cmath>


namespace tama {
    FractalAdaptiveMovingAverage::FractalAdaptiveMovingAverage(uint16_t period, double eulerNumber)
    : period(period), 
    eulerNumber(eulerNumber),  
    halfPeriod(period/2), 
    logTwo(log(2)), 
    highBuf1(period/2), 
    highBuf2(period/2), 
    lowBuf1(period/2), 
    lowBuf2(period/2) {}

    double FractalAdaptiveMovingAverage::latest() {
        return this->lastFrama;
    }


    // edge case: what if period is uneven? how wil window splitting be handled
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

        for (size_t i = 0; i < this->period; i ++) {
            output[i] = close[i];
        }

        std::span<const double> windowOneHigh =  high.subspan(this->period - this->period, this->halfPeriod);
        this->highBuf1Max = *std::max_element(windowOneHigh.begin(), windowOneHigh.end());
        this->highBuf1.insert(windowOneHigh);
        
        std::span<const double> windowTwoHigh = high.subspan(this->period - this->halfPeriod, this->halfPeriod);
        this->highBuf2Max = *std::max_element(windowTwoHigh.begin(), windowTwoHigh.end());
        this->highBuf2.insert(windowTwoHigh);


        std::span<const double> windowOneLow =  low.subspan(this->period - this->period, this->halfPeriod);
        this->lowBuf1min = *std::min_element(windowOneLow.begin(), windowOneLow.end());
        this->lowBuf1.insert(windowOneLow);

        std::span<const double> windowTwoLow = low.subspan(this->period - this->halfPeriod, this->halfPeriod);
        this->lowBuf2min = *std::min_element(windowTwoLow.begin(), windowTwoLow.end());
        this->lowBuf2.insert(windowTwoLow);

        for (size_t i = this->period; i <  closeLen; i++) {
            double fullWindowHigh =  this->highBuf1Max > this->highBuf2Max ? this->highBuf1Max : this->highBuf2Max;
            double fullWindowLow =  this->lowBuf1min < this->lowBuf2min ? this->lowBuf1min : this->lowBuf2min;

            double l1 = (this->highBuf1Max - this->lowBuf1min) / this->halfPeriod;
            double l2 = (this->highBuf2Max - this->lowBuf2min) / this->halfPeriod;
            double l3 = (fullWindowHigh - fullWindowLow) / this->period;
            double D = log((l1 + l2) / l3) / this->logTwo;
            if (D <= 0) D = 1;
            
            double alpha = exp(this->eulerNumber * (D - 1));

            output[i] = alpha * close[i] + (1-alpha) * output[i-1];

            const double hb1_head = highBuf1.head();
            const double hb2_head = highBuf2.head();

            const bool recalchb1 = (hb1_head == highBuf1Max);
            highBuf1.insert(hb2_head);

            if (recalchb1) {
                highBuf1Max = highBuf1.max();
            } else if (hb2_head > highBuf1Max) {
                highBuf1Max = hb2_head;
            }

            const bool recalchb2 = (hb2_head == highBuf2Max);
            highBuf2.insert(high[i]);

            if (recalchb2) {
                highBuf2Max = highBuf2.max();
            } else if (high[i] > highBuf2Max) {
                highBuf2Max = high[i];
            }

            const double lb1_head = lowBuf1.head();
            const double lb2_head = lowBuf2.head();

            const bool recalclb1 = (lb1_head == lowBuf1min);
            lowBuf1.insert(lb2_head);

            if (recalclb1) {
                lowBuf1min = lowBuf1.min();
            } else if (lb2_head < lowBuf1min) {
                lowBuf1min = lb2_head;
            }

            const bool recalclb2 = (lb2_head == lowBuf2min);
            lowBuf2.insert(low[i]);

            if (recalclb2) {
                lowBuf2min = lowBuf2.min();
            } else if (low[i] < lowBuf2min) {
                lowBuf2min = low[i];
            }
        }
        
        this->lastFrama = output.back();
        return status::ok;
    }

    double FractalAdaptiveMovingAverage::update(double close, double low, double high) {
        if (this->initialized) {
            throw std::runtime_error("frama not initialized");
        }
        
        double fullWindowHigh =  this->highBuf1Max > this->highBuf2Max ? this->highBuf1Max : this->highBuf2Max;
        double fullWindowLow =  this->lowBuf1min < this->lowBuf2min ? this->lowBuf1min : this->lowBuf2min;

        double l1 = (this->highBuf1Max - this->lowBuf1min) / this->halfPeriod;
        double l2 = (this->highBuf2Max - this->lowBuf2min) / this->halfPeriod;
        double l3 = (fullWindowHigh - fullWindowLow) / this->period;
        double D = log((l1 + l2) / l3) / this->logTwo;
        if (D <= 0) D = 1;
        
        double alpha = exp(this->eulerNumber * (D - 1));

        double out = alpha * close + (1-alpha) * this->lastFrama;

        const double hb1_head = highBuf1.head();
        const double hb2_head = highBuf2.head();

        const bool recalchb1 = (hb1_head == highBuf1Max);
        highBuf1.insert(hb2_head);

        if (recalchb1) {
            highBuf1Max = highBuf1.max();
        } else if (hb2_head > highBuf1Max) {
            highBuf1Max = hb2_head;
        }

        const bool recalchb2 = (hb2_head == highBuf2Max);
        highBuf2.insert(high);

        if (recalchb2) {
            highBuf2Max = highBuf2.max();
        } else if (high > highBuf2Max) {
            highBuf2Max = high;
        }

        const double lb1_head = lowBuf1.head();
        const double lb2_head = lowBuf2.head();

        const bool recalclb1 = (lb1_head == lowBuf1min);
        lowBuf1.insert(lb2_head);

        if (recalclb1) {
            lowBuf1min = lowBuf1.min();
        } else if (lb2_head < lowBuf1min) {
            lowBuf1min = lb2_head;
        }

        const bool recalclb2 = (lb2_head == lowBuf2min);
        lowBuf2.insert(low);

        if (recalclb2) {
            lowBuf2min = lowBuf2.min();
        } else if (low < lowBuf2min) {
            lowBuf2min = low;
        }

        this->lastFrama = out;
        return out;
    }
}