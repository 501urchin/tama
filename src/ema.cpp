#include <vector>
#include <tama/tama.hpp>

using namespace std;


namespace tama {
    void ema(const vector<double>& prices, vector<double>& ema, uint16_t emaPeriod) {
        size_t pricesLen = prices.size();
        if (ema.size() != pricesLen) ema.resize(pricesLen);
        ema[0] = prices[0];
    
        const double alpha = 2 / (double(emaPeriod) + 1);
        const double oma = 1 - alpha;

        for (size_t t = 1; t < pricesLen; t++) {
            ema[t] = alpha * prices[t] + oma * ema[t-1];
        }
    
        return;
    }
}

