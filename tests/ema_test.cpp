#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using namespace tama;


// values come from https://goodcalculators.com/exponential-moving-average-calculator
TEST(TamaTest, EmaMatchesKnownValues_test) {
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

TEST(TamaTest, EmaComputeThenUpdateMatchesKnownValues_test) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11,11.5,12.75,15.375,13.688,14.344,13.672,14.836,12.418};
    const size_t split = 5;
    vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> mixedOut;

    ExponentialMovingAverage ema(3);
    ASSERT_EQ(ema.compute(initialPrices, mixedOut), status::ok);

    for (size_t i = 0; i < split; i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }

    for (size_t i = split; i < prices.size(); i++) {
        mixedOut.push_back(ema.update(prices[i]));
    }

    ASSERT_EQ(mixedOut.size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, EmaRejectsemptyParams_test) {
    const vector<double> prices{};
    vector<double> emaOut{1.0, 2.0};

    ExponentialMovingAverage ema(30);
    status s = ema.compute(prices, emaOut);

    EXPECT_EQ(s, status::emptyParams);
    EXPECT_EQ(emaOut.size(), 2u);
}

TEST(TamaTest, EmaConstructorUsesPreviousCalculationForUpdate_test) {
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