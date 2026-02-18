#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>
#include <stdexcept>

using std::vector;

using tama::DoubleExponentialMovingAverage;

TEST(TamaTest, DemaMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{10, 11.5, 11.25, 12.625, 12.3125, 13.6562, 14.8281, 13.6641, 13.957, 15.541};
    vector<double> demaOut;

    DoubleExponentialMovingAverage dema(3);
    dema.compute(prices, demaOut);

    ASSERT_EQ(demaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(demaOut[i], expected[i], 1e-3) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, DemaRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> demaOut{1.0, 2.0};

    DoubleExponentialMovingAverage dema(3);
    const auto result = dema.compute(prices, demaOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(demaOut.size(), 2u);
}

TEST(TamaTest, DemaRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    vector<double> demaOut;

    EXPECT_THROW(DoubleExponentialMovingAverage(0), std::invalid_argument);
}
