#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>
#include <stdexcept>

using std::vector;

using tama::TripleExponentialMovingAverage;

TEST(TamaTest, TemaMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{10, 11.75, 11.25, 12.8125, 12.25, 13.7969, 14.9844, 13.4102, 13.8516, 15.7178};
    vector<double> temaOut;

    TripleExponentialMovingAverage tema(3);
    tema.compute(prices, temaOut);

    ASSERT_EQ(temaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(temaOut[i], expected[i], 1e-3) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, TemaRejectsEmptyParams_test) {
    const vector<double> prices{};
    vector<double> temaOut{1.0, 2.0};

    TripleExponentialMovingAverage tema(3);
    const auto result = tema.compute(prices, temaOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(temaOut.size(), 2u);
}

TEST(TamaTest, TemaRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    vector<double> temaOut;

    EXPECT_THROW(TripleExponentialMovingAverage(0), std::invalid_argument);
}
