#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>
#include <stdexcept>

using std::vector;

TEST(TamaTest, HullMatchesKnownValuesPeriod3_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{0, 0, 0, 12.8333, 12.5, 13.8333, 15.5, 13.3889, 13.5, 16.1667};
    vector<double> hullOut;

    auto result = tama::HullMovingAverage(3).compute(prices, hullOut);

    ASSERT_EQ(result, status::ok);
    ASSERT_EQ(hullOut.size(), prices.size());
    ASSERT_EQ(expected.size(), prices.size());


    for (size_t i = 3; i < expected.size(); ++i) {
        EXPECT_NEAR(hullOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, HullMatchesKnownValuesPeriod5_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{0, 0, 0, 0, 0, 13.5333, 15.1333, 14.4, 13.6, 15.2222};
    vector<double> hullOut;

    auto result = tama::HullMovingAverage(5).compute(prices, hullOut);

    ASSERT_EQ(result, status::ok);
    ASSERT_EQ(hullOut.size(), prices.size());
    ASSERT_EQ(expected.size(), prices.size());

    for (size_t i = 5; i < expected.size(); ++i) {
        EXPECT_NEAR(hullOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, HullComputeThenUpdateMatchesKnownValuesPeriod3_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{0, 0, 0, 12.8333, 12.5, 13.8333, 15.5, 13.3889, 13.5, 16.1667};
    const size_t period = 3;
    const size_t split = 6;

    vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> mixedOut;

    tama::HullMovingAverage hma(period);
    ASSERT_EQ(hma.compute(initialPrices, mixedOut), status::ok);

    for (size_t i = period; i < split; i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }

    for (size_t i = split; i < prices.size(); i++) {
        mixedOut.push_back(hma.update(prices[i]));
    }

    ASSERT_EQ(mixedOut.size(), expected.size());
    for (size_t i = period; i < expected.size(); i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, HullConstructorUsesPreviousWindowForUpdate_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const size_t period = 3;
    const double newPrice = 17.0;
    vector<double> hullOut;

    tama::HullMovingAverage baseline(period);
    ASSERT_EQ(baseline.compute(prices, hullOut), status::ok);

    vector<double> prevWindow(prices.end() - static_cast<std::ptrdiff_t>(period), prices.end());
    tama::HullMovingAverage resumed(period, prevWindow);

    const double baselineUpdated = baseline.update(newPrice);
    const double resumedUpdated = resumed.update(newPrice);

    EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}

TEST(TamaTest, HullRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> hullOut{1.0, 2.0};

    auto result = tama::HullMovingAverage(3).compute(prices, hullOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(hullOut.size(), 2u);
}

TEST(TamaTest, HullRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    vector<double> hullOut;

    EXPECT_THROW(tama::HullMovingAverage(0), std::invalid_argument);
}
