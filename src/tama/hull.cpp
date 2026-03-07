#include <vector>
#include <tama/tama.hpp>
#include <helpers/helpers.hpp>
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace {
    uint16_t require_period(uint16_t period) {
        if (period == 0) {
            throw std::invalid_argument("invalid period");
        }
        return period;
    }
}

tama::HullMovingAverage::HullMovingAverage(uint16_t period, std::vector<double> prevCalc)
    : p1(std::max<uint16_t>(1, static_cast<uint16_t>(require_period(period) / 2))),
      p2(std::max<uint16_t>(1, static_cast<uint16_t>(std::lround(std::sqrt(static_cast<double>(require_period(period))))))),
      period(require_period(period)),
      w1(p1),
      w2(period),
      w3(p2) {
    if (!prevCalc.empty()) {
        if (prevCalc.size() != this->period) {
            throw std::invalid_argument("prevCalc buffer doesn't match period");
        }
        std::vector<double> w1Out(prevCalc.size());
        std::vector<double> w2Out(prevCalc.size());
        std::vector<double> w3Out(prevCalc.size());

        status res = this->w1.compute(prevCalc, w1Out);
        if (res != status::ok) {
            return;
        }

        res = this->w2.compute(prevCalc, w2Out);
        if (res != status::ok) {
            return;
        }

        for (size_t i = 0; i < prevCalc.size(); ++i) {
            w1Out[i] = 2.0 * w1Out[i] - w2Out[i];
        }

        res = this->w3.compute(w1Out, w3Out);
        if (res != status::ok) {
            return;
        }

        this->lastHull = w3Out.back();
        this->initialized = true;
    }
}

tama::HullMovingAverage::HullMovingAverage(HullMovingAverageState prevCalculation)
    : p1(prevCalculation.p1),
      p2(prevCalculation.p2),
      period(prevCalculation.period),
      lastHull(prevCalculation.lastHull),
      initialized(prevCalculation.initialized),
      w1(prevCalculation.w1),
      w2(prevCalculation.w2),
      w3(prevCalculation.w3) {
    if (this->period == 0) {
        throw std::invalid_argument("invalid period");
    }

    const uint16_t expectedP1 = std::max<uint16_t>(1, static_cast<uint16_t>(this->period / 2));
    const uint16_t expectedP2 = std::max<uint16_t>(1, static_cast<uint16_t>(std::lround(std::sqrt(static_cast<double>(this->period)))));

    if (this->p1 != expectedP1 || this->p2 != expectedP2) {
        throw std::invalid_argument("invalid derived periods in HMA state");
    }
}

status tama::HullMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty()) {
        return status::emptyParams;
    }

    const size_t pricesLen = prices.size();
    if (output.size() != pricesLen) {
        output.resize(pricesLen);
    }

    std::vector<double> wmaAOut(pricesLen);
    std::vector<double> wmaBOut(pricesLen);


    status res = this->w1.compute(prices, wmaAOut);
    if (res != status::ok) {
        return res;
    }
    
    res = this->w2.compute(prices, wmaBOut);
    if (res != status::ok) {
        return res;
    }


    // simd
    for (size_t i = 0; i < pricesLen; ++i) {
        wmaAOut[i] = 2.0 * wmaAOut[i] - wmaBOut[i];
    }

    res = this->w3.compute(wmaAOut, output);
    if (res != status::ok) {
        return res;
    }
    
    this->lastHull = output.back();
    this->initialized = true;
    
    return status::ok;
}

double tama::HullMovingAverage::latest() {
    return this->lastHull;
}

HullMovingAverageState tama::HullMovingAverage::getState() {
    return {
        .p1 = this->p1,
        .p2 = this->p2,
        .period = this->period,
        .lastHull = this->lastHull,
        .initialized = this->initialized,
        .w1 = this->w1.getState(),
        .w2 = this->w2.getState(),
        .w3 = this->w3.getState()
    };
}

double tama::HullMovingAverage::update(double price) {
    if (!this->initialized) {
        throw std::runtime_error("hma not initialized");
    }

    double w1 = this->w1.update(price);
    double w2 = this->w2.update(price);

    double res = this->w3.update(2 * w1 - w2);
    this->lastHull = res;
    return res;
}