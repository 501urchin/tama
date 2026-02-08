#include <vector>
#include <tama/tama.hpp>

using namespace std;


namespace tama {
    status ema(const vector<double>& prices, vector<double>& emaOut, const uint16_t emaPeriod) {
        if (prices.empty() ) {
            return status::emptyPrices;
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

