#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::ema;
using tama::status;


// values come from https://goodcalculators.com/exponential-moving-average-calculator
TEST(TamaTest, EmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11,11.5,12.75,15.375,13.688,14.344,13.672,14.836,12.418};
    vector<double> emaOut;

    ema(prices, emaOut, 3);

    ASSERT_EQ(emaOut.size(), prices.size()) << "Vectors differ in size";
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(emaOut[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, EmaRejectsEmptyPrices) {
    const vector<double> prices{};
    vector<double> emaOut{1.0, 2.0};

    const auto result = ema(prices, emaOut, 3);

    EXPECT_EQ(result, tama::status::emptyPrices);
    EXPECT_EQ(emaOut.size(), 2u);
}


TEST(TamaTest, EmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> emaOut;

    const auto result = ema(prices, emaOut, 0);

    EXPECT_EQ(result, tama::status::invalidParam);
}