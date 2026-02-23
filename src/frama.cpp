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

    double FractalAdaptiveMovingAverage::update(double close, double low, double high) {
        return 0.0;
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

        for (size_t i = 0; i < this->period; i ++) {
            output[i] = close[i];
        }

        // edge case: what if period is uneven? how wil window splitting be handled

        // idea: put these in the class
        double hb1Max;
        helpers::RingBuffer<double> hb1(this->halfPeriod);
        double hb2Max;
        helpers::RingBuffer<double> hb2(this->halfPeriod);

        double lb1Min;
        helpers::RingBuffer<double> lb1(this->halfPeriod);
        double lb2Min;
        helpers::RingBuffer<double> lb2(this->halfPeriod);


        std::span<const double> windowOneHigh =  high.subspan(this->period - this->period, this->halfPeriod);
        hb1Max = *std::max_element(windowOneHigh.begin(), windowOneHigh.end());
        hb1.insert(windowOneHigh);
        
        std::span<const double> windowTwoHigh = high.subspan(this->period - this->halfPeriod, this->halfPeriod);
        hb2Max = *std::max_element(windowTwoHigh.begin(), windowTwoHigh.end());
        hb2.insert(windowTwoHigh);


        std::span<const double> windowOneLow =  low.subspan(this->period - this->period, this->halfPeriod);
        lb1Min = *std::min_element(windowOneLow.begin(), windowOneLow.end());
        lb1.insert(windowOneLow);

        std::span<const double> windowTwoLow = low.subspan(this->period - this->halfPeriod, this->halfPeriod);
        lb2Min = *std::min_element(windowTwoLow.begin(), windowTwoLow.end());
        lb2.insert(windowTwoLow);

        for (size_t i = this->period; i <  closeLen; i++) {

            // this can be sped up by trying to avoid recalc every iteration
            // only recalc if leaving == max


            double fullWindowHigh =  hb1Max > hb2Max ? hb1Max : hb2Max;
            double fullWindowLow =  lb1Min < lb2Min ? lb1Min : lb2Min;

            double l1 = (hb1Max - lb1Min) / this->halfPeriod;
            double l2 = (hb2Max - lb2Min) / this->halfPeriod;
            double l3 = (fullWindowHigh - fullWindowLow) / this->period;
            double D = log((l1 + l2) / l3) / this->logTwo;
            if (D <= 0) D = 1;
            
            double alpha = exp(this->eulerNumber * (D - 1));

            output[i] = alpha * close[i] + (1-alpha) * output[i-1];

            bool recalchb1 = hb1.head() == hb1Max;
            hb1.insert(hb2.head());
            if (recalchb1) {
                hb1Max = hb1.max();
            } else if (hb2.head() > hb1Max) {
                hb1Max = hb2.head();
            }
            
            bool recalchb2 = hb2.head() == hb2Max;
            hb2.insert(high[i]);
            if (recalchb2) {
                hb2Max = hb2.max();
            } else if (high[i] > hb2Max) {
                hb2Max = high[i];
            }
            

            bool recalclb1 = lb1.head() == lb1Min;
            lb1.insert(lb2.head());
            if (recalclb1) {
                lb1Min = lb1.min();
            } else if (lb2.head() < lb1Min) {
                lb1Min = lb2.head();
            }

            bool recalclb2 = lb2.head() == lb2Min;
            lb2.insert(low[i]);
            if (recalclb2) {
                lb2Min = lb2.min();
            } else if (low[i] < lb2Min) {
                lb2Min = low[i];
            }

            

        }
        
        return status::ok;
    }
}