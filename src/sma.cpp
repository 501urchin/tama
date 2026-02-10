#include <vector>
#include <tama/tama.hpp>
#include "helpers.cpp"
#include <algorithm>
#include <span>

using namespace std;

namespace tama {    
    status sma(std::span<const double> prices, vector<double>& smaOut, const uint16_t smaPeriod) {
        if (prices.empty() ) {
            return status::emptyParams;
        }
        size_t pricesLen = prices.size();

        if (smaPeriod >= pricesLen || smaPeriod == 0 ) {
            return status::invalidParam;
        }

        if (smaOut.size() != pricesLen) {
            smaOut.resize(pricesLen);
        }
        std::fill(smaOut.begin(), smaOut.begin() + smaPeriod-1, 0);

        const double alpha = 1.0 / static_cast<double>(smaPeriod);
        double sum = helpers::simdSum<double>(prices.subspan(0, smaPeriod));
        smaOut[smaPeriod-1] = alpha * sum;
        
        for (size_t t = smaPeriod; t < pricesLen; t++ ) {
            sum += prices[t] - prices[t-smaPeriod];
            smaOut[t] = alpha * sum;
        }
        
        return status::ok;
    }
}