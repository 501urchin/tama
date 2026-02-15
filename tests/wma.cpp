#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;



TEST(TamaTest, WmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{0, 0, 12.8333, 15.6667, 14.3333 ,14.5, 13.5 ,14.8333 ,12.5};
    vector<double> wmaOut;

    tama::WeightedMovingAverage stateful(3);
    stateful.compute(prices, wmaOut);

    ASSERT_EQ(wmaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(wmaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}




TEST(TamaTest, WmaRejectsemptyParams) {
    const vector<double> prices{};
    vector<double> wmaOut{1.0, 2.0};

    tama::WeightedMovingAverage stateful(3);
    status result = stateful.compute(prices, wmaOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(wmaOut.size(), 2u);
}


TEST(TamaTest, WmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> wmaOut;

    tama::WeightedMovingAverage stateful(0);
    status result = stateful.compute(prices, wmaOut);

    EXPECT_EQ(result, status::invalidParam);
}

TEST(TamaTest, WmaConstructorUsesPreviousWindowForUpdate) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const size_t period = 3;
    const double newPrice = 19.0;
    vector<double> wmaOut;

    tama::WeightedMovingAverage baseline(period);
    ASSERT_EQ(baseline.compute(prices, wmaOut), status::ok);

    vector<double> prevWindow(prices.end() - static_cast<std::ptrdiff_t>(period), prices.end());
    tama::WeightedMovingAverage resumed(period, prevWindow);

    const double baselineUpdated = baseline.update(newPrice);
    const double resumedUpdated = resumed.update(newPrice);

    EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}