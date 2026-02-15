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

TEST(TamaTest, WmaMultipleUpdatesMatchRecompute) {
    const vector<double> prices{10, 11, 12, 13, 14, 15, 16};
    const vector<double> updates{17.0, 18.0, 19.0, 20.0};
    const size_t period = 4;

    vector<double> baselineOut;
    tama::WeightedMovingAverage baseline(period);
    ASSERT_EQ(baseline.compute(prices, baselineOut), status::ok);

    vector<double> rollingSeries = prices;
    tama::WeightedMovingAverage stateful(period);
    ASSERT_EQ(stateful.compute(prices, baselineOut), status::ok);

    for (double updatePrice : updates) {
        const double statefulValue = stateful.update(updatePrice);

        rollingSeries.push_back(updatePrice);
        vector<double> recomputed;
        tama::WeightedMovingAverage reference(period);
        ASSERT_EQ(reference.compute(rollingSeries, recomputed), status::ok);

        EXPECT_NEAR(statefulValue, recomputed.back(), 1e-12);
    }
}

TEST(TamaTest, WmaComputeThenUpdateMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{0, 0, 12.8333, 15.6667, 14.3333 ,14.5, 13.5 ,14.8333 ,12.5};
    const size_t period = 3;
    const size_t split = 5;

    vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> mixedOut;

    tama::WeightedMovingAverage wma(period);
    ASSERT_EQ(wma.compute(initialPrices, mixedOut), status::ok);

    for (size_t i = 0; i < split; i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }

    for (size_t i = split; i < prices.size(); i++) {
        mixedOut.push_back(wma.update(prices[i]));
    }

    ASSERT_EQ(mixedOut.size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}
