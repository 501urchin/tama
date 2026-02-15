#include <vector>
#include <tama/tama.hpp>
#include <cmath>
#include <algorithm>

using namespace std;

namespace tama {
    status dema(std::span<const double> prices, std::vector<double>& demaOut, const uint16_t demaPeriod) {
        if (prices.empty()) {
            return status::emptyParams;
        }
    
        if (demaPeriod == 0) {
            return status::invalidParam;
        }
    
        const size_t pricesLen = prices.size();
        if (demaOut.size() < pricesLen) {
            demaOut.resize(pricesLen);
        }
    
        vector<double> emaAOut(pricesLen);
        vector<double> emaBOut(pricesLen);

        ExponentialMovingAverage ema(demaPeriod);
        status res = ema.compute(prices, emaAOut);
        if (res != status::ok) {
            return res;
        }
    
        ExponentialMovingAverage ema2(demaPeriod);
        res = ema2.compute(emaAOut, emaBOut);
        if (res != status::ok) {
            return res;
        }
    
        // simd
        for (size_t i = 0; i < pricesLen; i++) {
            demaOut[i] = 2 * emaAOut[i] - emaBOut[i];
        }
    
        return status::ok;
    }

    status tema(std::span<const double> prices, std::vector<double>& temaOut, const uint16_t temaPeriod) {
        if (prices.empty()) {
            return status::emptyParams;
        }
    
        if (temaPeriod == 0) {
            return status::invalidParam;
        }
    
        const size_t pricesLen = prices.size();
        if (temaOut.size() != pricesLen) {
            temaOut.resize(pricesLen);
        }
    
        vector<double> emaAOut(pricesLen);
        vector<double> emaBOut(pricesLen);
        vector<double> emaCOut(pricesLen);
    

        ExponentialMovingAverage ema(temaPeriod);
        status res = ema.compute(prices, emaAOut);
        if (res != status::ok) {
            return res;
        }
    
        ExponentialMovingAverage ema2(temaPeriod);
        res = ema2.compute(emaAOut, emaBOut);
        if (res != status::ok) {
            return res;
        }
    
        ExponentialMovingAverage ema3(temaPeriod);
        res = ema2.compute(emaBOut, emaCOut);
        if (res != status::ok) {
            return res;
        }
    
    
        // simd
        for (size_t i = 0; i < pricesLen; i++) {
            temaOut[i] = 3 * emaAOut[i] - 3 * emaBOut[i] + emaCOut[i];
        }
    
        return status::ok;
    }

}

tama::HullMovingAverage::HullMovingAverage(uint16_t period, std::vector<double> prevCalc)
        : p1(std::max<uint16_t>(1, static_cast<uint16_t>(period / 2))),
            p2(std::max<uint16_t>(1, static_cast<uint16_t>(std::lround(std::sqrt(static_cast<double>(period)))))),
            period(period),
            w1(p1),
            w2(period),
            w3(p2) {

    if (!prevCalc.empty()) {
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

status tama::HullMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
    if (prices.empty()) {
        return status::emptyParams;
    }

    if (this->period == 0) {
        return status::invalidParam;
    }

    const size_t pricesLen = prices.size();
    if (output.size() != pricesLen) {
        output.resize(pricesLen);
    }

    vector<double> wmaAOut(pricesLen);
    vector<double> wmaBOut(pricesLen);


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
    if (!this->initialized) {
        this->initialized = true;
    }
    
    return status::ok;
}

double tama::HullMovingAverage::latest() {
    return this->lastHull;
}

double tama::HullMovingAverage::update(double price) {

    double w1 = this->w1.update(price);
    double w2 = this->w2.update(price);

    double res =  this->w3.update(2 * w1 - w2);
    this->lastHull = res;
    return res;
}