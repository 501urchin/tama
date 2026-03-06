#include <tama/tama.hpp>
#include <helpers/helpers.hpp>



namespace tama {
    GeneralizedDoubleExponentialMovingAverage::GeneralizedDoubleExponentialMovingAverage(double period, uint16_t emaPeriod): period(period), oneMinPeriod(1-period), emaBuf1(emaPeriod), emaBuf2(emaPeriod) {};
}