#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>
#include <stdexcept>

using std::vector;

using tama::McGinleyDynamicMovingAverage;

TEST(TamaTest, MdMatchesKnownValues_test) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11.0000, 11.0708, 11.1851, 11.2866, 11.3425, 11.4621, 11.5551, 11.6758, 11.3634};
    vector<double> mdOut;

    McGinleyDynamicMovingAverage md(10);
    const auto result = md.compute(prices, mdOut);

    EXPECT_EQ(result, status::ok);
    ASSERT_EQ(mdOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mdOut[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, MdRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> mdOut{1.0, 2.0};

    McGinleyDynamicMovingAverage md(3);
    const auto result = md.compute(prices, mdOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(mdOut.size(), 2u);
}

TEST(TamaTest, MdRejectsInvalidParams_test) {
    EXPECT_THROW(McGinleyDynamicMovingAverage(0), std::invalid_argument);
}

TEST(TamaTest, MdComputeThenUpdateMatchesKnownValues_test) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11.0000, 11.0708, 11.1851, 11.2866, 11.3425, 11.4621, 11.5551, 11.6758, 11.3634};
    const size_t split = 5;

    vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> mixedOut;

    McGinleyDynamicMovingAverage md(10);
    ASSERT_EQ(md.compute(initialPrices, mixedOut), status::ok);

    for (size_t i = 0; i < split; i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }

    for (size_t i = split; i < prices.size(); i++) {
        mixedOut.push_back(md.update(prices[i]));
    }

    ASSERT_EQ(mixedOut.size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}
