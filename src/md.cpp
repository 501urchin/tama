#include <tama/tama.hpp>
#include <cmath>





namespace tama {
    status md(std::span<const double> prices, std::vector<double>& mdOut, uint16_t mdPeriod) {
        if (prices.empty() ) {
            return status::emptyParams;
        }

        if (mdPeriod == 0) {
            return status::invalidParam;
        }
        size_t pricesLen = prices.size();

        if (mdOut.size() != pricesLen) {
            mdOut.resize(pricesLen);
        }

        mdOut[0] = prices[0];

        for (size_t t = 1; t < pricesLen; t++) {
            double pt = prices[t];
            double mt = mdOut[t-1];

            double numerator = pt - mt;
            double denominator = static_cast<double>(mdPeriod) * pow(pt / mt, 4.0);

            mdOut[t] = numerator / denominator + mt;
        }

        return status::ok;
    }
}