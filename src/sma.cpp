#include <vector>
#include <tama/tama.hpp>
#include <algorithm>
#include <numeric>

using namespace std;

namespace tama {    
    status sma(const vector<double>& prices, vector<double>& smaOut, const uint16_t smaPeriod) {
        if (prices.empty() ) {
            return status::emptyPrices;
        }
        size_t pricesLen = prices.size();

        if (smaPeriod >= pricesLen || smaPeriod == 0 ) {
            return status::invalidParam;
        }

        if (smaOut.size() != pricesLen) {
            smaOut.resize(pricesLen);
        }
        std::fill(smaOut.begin(), smaOut.begin() + smaPeriod-1, 0);

        const double alpha = 1 / double(smaPeriod);
        double sum = 0.0;
        
        
        for (uint16_t i = 0; i < smaPeriod; i++ ) {
            sum += prices[smaPeriod-i-1];
        }
        smaOut[smaPeriod-1] = alpha * sum;
        
        for (size_t t = smaPeriod; t < pricesLen; t++ ) {
            sum += prices[t] - prices[t-smaPeriod];
            smaOut[t] = alpha * sum;
        }
        
        return status::ok;
    }
}