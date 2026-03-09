#include <tama/tama.hpp>
#include <helpers/helpers.hpp>



namespace tama {
    GeneralizedDoubleExponentialMovingAverage::GeneralizedDoubleExponentialMovingAverage(double period, uint16_t emaPeriod): period(period), emaPeriod(emaPeriod), onePlusPeriod(1+period), emaBuf1(emaPeriod), emaBuf2(emaPeriod) {
        if (period == 0) {
            throw std::invalid_argument("invalid period");
        }

        if (emaPeriod == 0) {
            throw std::invalid_argument("invalid emaPeriod");
        }
    };


    status GeneralizedDoubleExponentialMovingAverage::compute(std::span<const double> prices, std::vector<double>& output) {
        size_t priceLen = prices.size(); 

        if (output.size() < priceLen) {
            output.resize(priceLen);
        }

        std::vector<double> ema1Out(priceLen);
        status res = this->emaBuf1.compute(prices, ema1Out);
        if (res != status::ok) {
            return  res;
        }

        std::vector<double> ema2Out(priceLen);
        res = this->emaBuf2.compute(ema1Out, ema2Out);
        if (res != status::ok) {
            return  res;
        }

        for (size_t i = 0; i < priceLen; i++) {
            output[i] = this->onePlusPeriod * ema1Out[i] - this->period * ema2Out[i];
        }

        this->lastGd = output.back();
    }


    double GeneralizedDoubleExponentialMovingAverage::latest() {
        return this->lastGd;
    } 


    double GeneralizedDoubleExponentialMovingAverage::update(double price) {
        double ema1res = this->emaBuf1.update(price);
        double ema2res = this->emaBuf2.update(ema1res);

        double NewGd = this->onePlusPeriod * ema1res - this->period * ema2res;
        this->lastGd = NewGd;

        return NewGd;
    };

}