#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using namespace tama;


TEST(TamaTest, FramaMatchesKnownValues) {
    std::vector<double> high = {0.755156, 0.639031, 0.752145, 0.136273, 0.903269, 0.0940683, 0.57457, 0.372888, 0.273874, 0.390271};
    std::vector<double> low  = {0.745000, 0.629000, 0.742000, 0.130000, 0.893000, 0.090000, 0.564000, 0.362000, 0.263000, 0.380000};
    std::vector<double> close= {0.750000, 0.635000, 0.748000, 0.133000, 0.898000, 0.092000, 0.569000, 0.367000, 0.268000, 0.385000};
    std::vector<double> expect = {0.750000, 0.635000, 0.748000, 0.133000, 0.364974, 0.262595, 0.269655, 0.275043, 0.273535, 0.302247};
    std::vector<double> out;

    uint16_t period = 4;
    
    FractalAdaptiveMovingAverage frama(period);
    status res = frama.compute(close, low, high, out);

    ASSERT_EQ(res, status::ok) << "Got non status::ok response from func";
    ASSERT_EQ(out.size(), close.size()) << "Vectors differ in size";

    for (size_t i = 0; i < expect.size(); i++) {
        if (i < period) {
            EXPECT_NEAR(out[i], close[i], 1e-1) << "Vectors differ at index " << i;
        }else {
            EXPECT_NEAR(out[i], expect[i], 1e-1) << "Vectors differ at index " << i;
        }
    }
}

TEST(TamaTest, FramaRollingUpdate) {
    std::vector<double> high = {0.755156, 0.639031, 0.752145, 0.136273, 0.903269, 0.0940683, 0.57457, 0.372888, 0.273874, 0.390271};
    std::vector<double> low  = {0.745000, 0.629000, 0.742000, 0.130000, 0.893000, 0.090000, 0.564000, 0.362000, 0.263000, 0.380000};
    std::vector<double> close= {0.750000, 0.635000, 0.748000, 0.133000, 0.898000, 0.092000, 0.569000, 0.367000, 0.268000, 0.385000};
    std::vector<double> expect = {0.750000, 0.635000, 0.748000, 0.133000, 0.364974, 0.262595, 0.269655, 0.275043, 0.273535, 0.302247};
    std::vector<double> out;

    uint16_t period = 4;

    FractalAdaptiveMovingAverage frama(period);
    status res = frama.compute(close, low, high, out);
    
    ASSERT_EQ(res, status::ok) << "Got non status::ok response from func";
    ASSERT_EQ(out.size(), close.size()) << "Vectors differ in size";

    
    double nextHigh = 0.2947;
    double nextLow = 0.2847;
    double nextClose = 0.2900;
    double nextExp = 0.295365;

    double next = frama.update(nextClose, nextLow, nextHigh);

    EXPECT_NEAR(next, nextExp, 1e-1) << "got: " << next << " doesnt match expected: " << nextExp;
}