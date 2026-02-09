#include <vector>
#include <tama/tama.hpp>

using namespace std;

namespace tama {    
     status vwma(std::span<const double> prices, std::span<const double> volume, std::vector<double>& vwmaOut, const uint16_t vwmaPeriod) {
        size_t pricesLen = prices.size();
        size_t volumeLen = volume.size();

        if (pricesLen == 0 || volumeLen == 0) {
            return status::emptyParams;
        }

        if (pricesLen != volumeLen || vwmaPeriod == 0 || vwmaPeriod >= pricesLen) {
            return status::invalidParam;
        }

        if (vwmaOut.size() != pricesLen) {
            vwmaOut.resize(pricesLen);
        }


        double numeratorSum = 0;
        double denominatorSum = 0;

        for (size_t i = 0; i < vwmaPeriod; i++) {
            numeratorSum += prices[i] * volume[i];
            denominatorSum += volume[i];
        }
        vwmaOut[vwmaPeriod-1] = (numeratorSum) / denominatorSum;

        for (size_t t = vwmaPeriod; t < pricesLen; t++) {
            numeratorSum -= prices[t-vwmaPeriod] * volume[t-vwmaPeriod];
            numeratorSum += prices[t] * volume[t];
            
            denominatorSum -= volume[t-vwmaPeriod];
            denominatorSum += volume[t];
    

            vwmaOut[t] = numeratorSum  / denominatorSum;
        }

        return status::ok;
    }
}