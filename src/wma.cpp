#include <vector>
#include <tama/tama.hpp>

using namespace std;

namespace tama {    
    status wma(std::span<const double> prices, std::vector<double>& wmaOut, const uint16_t wmaPeriod) {
        size_t pricesLen = prices.size();

        if (pricesLen == 0) {
            return status::emptyParams;
        }

        if (wmaPeriod == 0 || wmaPeriod >= pricesLen) {
            return status::invalidParam;
        }

        if (wmaOut.size() < pricesLen) {
            wmaOut.resize(pricesLen);
        }

        double wSum = wmaPeriod * (wmaPeriod + 1) / 2;
        double sSum = 0;
        double weightedSum = 0;


        for (size_t i = 0; i < wmaPeriod; i++) {
            sSum += prices[i];
            weightedSum += prices[i] * (i+1);
        }
        wmaOut[wmaPeriod-1] = weightedSum / wSum;

        for (size_t t = wmaPeriod; t < pricesLen; t++) {
            weightedSum -= sSum;
            sSum -= prices[t-wmaPeriod];
            sSum += prices[t];
            weightedSum += prices[t] * wmaPeriod;

            wmaOut[t] = weightedSum / wSum;
        }

        return status::ok;
    }
}