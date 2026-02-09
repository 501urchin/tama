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

        

        return status::ok;
    }
}