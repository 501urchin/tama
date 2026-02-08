#include <vector>
#include <tama/tama.hpp>
#include <algorithm>

using namespace std;

namespace tama {    
    void sma(const vector<double>& prices, vector<double>& sma, uint16_t smaPeriod) {
        size_t pricesLen = prices.size();
        if (smaPeriod > pricesLen || smaPeriod == 0 ) {
            return;
        }
        
        if (sma.size() != pricesLen) {
            sma.resize(pricesLen);
        }
        std::fill(sma.begin(), sma.begin() + smaPeriod-1, 0);

        double alpha = 1 / double(smaPeriod);
        double sum = 0.0;
        
        for (size_t i = 0; i < smaPeriod; i++ ) {
            sum += prices[smaPeriod-i-1];
        }
        sma[smaPeriod-1] = alpha * sum;
        
        for (size_t t = smaPeriod; t < pricesLen; t++ ) {
            sum += prices[t] - prices[t-smaPeriod];
            sma[t] = alpha * sum;
        }
        
        return;
    }
}