#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::dema;
using tama::tema;


TEST(TamaTest, DemaMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{10, 11.5, 11.25, 12.625, 12.3125, 13.6562, 14.8281, 13.6641, 13.957, 15.541};
    vector<double> demaOut;

    dema(prices, demaOut, 3);

    ASSERT_EQ(demaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(demaOut[i], expected[i], 1e-3) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, DemaRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> demaOut{1.0, 2.0};

    const auto result = dema(prices, demaOut, 3);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(demaOut.size(), 2u);
}

TEST(TamaTest, DemaRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    vector<double> demaOut;

    EXPECT_EQ(dema(prices, demaOut, 0), status::invalidParam);
}

TEST(TamaTest, TemaMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{10, 11.75, 11.25, 12.8125, 12.25, 13.7969, 14.9844, 13.4102, 13.8516, 15.7178};
    vector<double> temaOut;

    tema(prices, temaOut, 3);

    ASSERT_EQ(temaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(temaOut[i], expected[i], 1e-3) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, TemaRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> temaOut{1.0, 2.0};

    const auto result = tema(prices, temaOut, 3);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(temaOut.size(), 2u);
}

TEST(TamaTest, TemaRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    vector<double> temaOut;

    EXPECT_EQ(tema(prices, temaOut, 0), status::invalidParam);
}


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
