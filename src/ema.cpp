#include <vector>
#include <tama/tama.hpp>

using namespace std;


namespace tama {
    // an idea could be to cache calculations. we create a obj containign the len of prices, emsValues, len period. 
    // on func call we check cache if no change to values we retrun the cache obj
    // if change to lets say price we calculate starting from last calcukated
    // if emaperiod changes we create a new calculation and new obj
    status ema(std::span<const double> prices, vector<double>& emaOut, const uint16_t emaPeriod) {
        if (prices.empty() ) {
            return status::emptyParams;
        }

        if (emaPeriod == 0) {
            return status::invalidParam;
        }
        size_t pricesLen = prices.size();

        if (emaOut.size() != pricesLen) {
            emaOut.resize(pricesLen);
        }
        emaOut[0] = prices[0];
    
        const double alpha = 2 / (double(emaPeriod) + 1);
        const double oma = 1 - alpha;

        for (size_t t = 1; t < pricesLen; t++) {
            emaOut[t] = alpha * prices[t] + oma * emaOut[t-1];
        }
    
        return status::ok;
    }
}

