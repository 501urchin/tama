#include <vector>
#include <tama/tama.hpp>

using namespace std;


// One approach is to cache calculations:
// - Create an object that stores the length of prices, EMA values, and the EMA period.
// - On function call, check the cache: 
//     - If input values haven’t changed, return the cached object.
//     - If new prices are added, continue calculations from the last computed point.
//     - If the EMA period changes, perform a full recalculation and create a new object.

// Another approach is to maintain a global structure:
// - Store all prices and a vector of EMA values.
// - When new prices are added (e.g., caller passes a span of length 3):
//     - Append the new prices to the global price vector.
//     - Calculate EMA only for the new entries, instead of recalculating the entire sequence.
namespace tama {
    status ema(std::span<const double> prices, vector<double>& emaOut, const uint16_t emaPeriod) {
        if (prices.empty() ) {
            return status::emptyParams;
        }

        if (emaPeriod == 0) {
            return status::invalidParam;
        }
        size_t pricesLen = prices.size();

        if (emaOut.size() < pricesLen) {
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

