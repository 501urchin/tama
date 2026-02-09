#include <vector>
#include <tama/tama.hpp>
#include <algorithm>

using namespace std;

namespace tama {    
    status wma(const std::vector<double>& prices, std::vector<double>& wmaOut, const uint16_t wmaPeriod) {
        size_t pricesLen = prices.size();

        if (pricesLen == 0) {
            return status::emptyPrices;
        }

        if (wmaPeriod == 0 || wmaPeriod >= pricesLen) {
            return status::invalidParam;
        }

        if (wmaOut.size() != pricesLen) {
            wmaOut.resize(pricesLen);
        }

        for (size_t t = wmaPeriod-1; t < pricesLen; t++) {
            double sum = 0;
            double weightSum = 0;
            for (size_t i = 0; i < wmaPeriod; i++) {
               double weight = wmaPeriod - i;
               weightSum += weight;
               sum += weight * prices[t-i];
            }

            wmaOut[t] = sum / weightSum;
        }


        return status::ok;
    }
}