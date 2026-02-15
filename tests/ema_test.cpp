#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using namespace tama;


// values come from https://goodcalculators.com/exponential-moving-average-calculator
TEST(TamaTest, EmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11,11.5,12.75,15.375,13.688,14.344,13.672,14.836,12.418};
    vector<double> emaOut;

    ExponentialMovingAverage ema(3);
    status s = ema.compute(prices, emaOut);


    ASSERT_EQ(emaOut.size(), prices.size()) << "Vectors differ in size";
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(emaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, EmaRejectsemptyParams) {
    const vector<double> prices{};
    vector<double> emaOut{1.0, 2.0};

    ExponentialMovingAverage ema(30);
    status s = ema.compute(prices, emaOut);

    EXPECT_EQ(s, status::emptyParams);
    EXPECT_EQ(emaOut.size(), 2u);
}

TEST(TamaTest, EmaConstructorUsesPreviousCalculationForUpdate) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const double newPrice = 19.0;
    vector<double> emaOut;

    ExponentialMovingAverage baseline(3);
    ASSERT_EQ(baseline.compute(prices, emaOut), status::ok);

    ExponentialMovingAverage resumed(3, emaOut.back());

    const double baselineUpdated = baseline.update(newPrice);
    const double resumedUpdated = resumed.update(newPrice);

    EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}