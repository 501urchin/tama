#include <vector>
#include <tama/tama.hpp>

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
        if (demaOut.size() != pricesLen) {
            demaOut.resize(pricesLen);
        }
    
        vector<double> emaAOut(pricesLen);
        vector<double> emaBOut(pricesLen);
    
        status res = ema(prices, emaAOut, demaPeriod);
        if (res != status::ok) {
            return res;
        }
    
        res = ema(emaAOut, emaBOut, demaPeriod);
        if (res != status::ok) {
            return res;
        }
    
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
    
        status res = ema(prices, emaAOut, temaPeriod);
        if (res != status::ok) {
            return res;
        }
    
        res = ema(emaAOut, emaBOut, temaPeriod);
        if (res != status::ok) {
            return res;
        }
    
        res = ema(emaBOut, emaCOut, temaPeriod);
        if (res != status::ok) {
            return res;
        }
    
        for (size_t i = 0; i < pricesLen; i++) {
            temaOut[i] = 3 * emaAOut[i] - 3 * emaBOut[i] + emaCOut[i];
        }
    
        return status::ok;
    }
}
