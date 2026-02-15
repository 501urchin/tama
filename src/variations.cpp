#include <vector>
#include <tama/tama.hpp>
#include <cmath>

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

    status hull(std::span<const double> prices, std::vector<double>& hullOut, const uint16_t hullPeriod) {
        if (prices.empty()) {
            return status::emptyParams;
        }
    
        if (hullPeriod == 0) {
            return status::invalidParam;
        }
    
        const size_t pricesLen = prices.size();
        if (hullOut.size() != pricesLen) {
            hullOut.resize(pricesLen);
        }

        vector<double> wmaAOut(pricesLen);
        vector<double> wmaBOut(pricesLen);

        uint16_t p1 = static_cast<uint16_t>(std::lround(hullPeriod / 2));
        status res = tama::WeightedMovingAverage(p1).compute(prices, wmaAOut);
        if (res != status::ok) {
            return res;
        }
        
        res = tama::WeightedMovingAverage(hullPeriod).compute(prices, wmaBOut);
        if (res != status::ok) {
            return res;
        }


        // simd
        for (size_t i = 0; i < pricesLen; ++i) {
            wmaAOut[i] = 2.0 * wmaAOut[i] - wmaBOut[i];
        }
        
        uint16_t p2 = static_cast<uint16_t>(std::lround((std::sqrt(hullPeriod))));
        return tama::WeightedMovingAverage(p2).compute(wmaAOut, hullOut);
    }
}
