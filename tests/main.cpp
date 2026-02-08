#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

// values come from https://goodcalculators.com/exponential-moving-average-calculator
TEST(TamaTest, EmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11,11.5,12.75,15.375,13.688,14.344,13.672,14.836,12.418};
    vector<double> ema;

    tama::ema(prices, ema, 3);

    ASSERT_EQ(ema.size(), prices.size()) << "Vectors differ in size";
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(ema[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, SmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{0,0,12.333,14.667,14.667,15,13.333,14.667,13};
    vector<double> sma;

    tama::sma(prices, sma, 3);

    ASSERT_EQ(sma.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(sma[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, EmaRejectsEmptyPrices) {
    const vector<double> prices{};
    vector<double> ema{1.0, 2.0};

    const auto result = tama::ema(prices, ema, 3);

    EXPECT_EQ(result, status::emptyPrices);
    EXPECT_EQ(ema.size(), 2u);
}

TEST(TamaTest, SmaRejectsEmptyPrices) {
    const vector<double> prices{};
    vector<double> sma{1.0, 2.0};

    const auto result = tama::sma(prices, sma, 3);

    EXPECT_EQ(result, status::emptyPrices);
    EXPECT_EQ(sma.size(), 2u);
}

TEST(TamaTest, EmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> ema;

    const auto result = tama::ema(prices, ema, 0);

    EXPECT_EQ(result, status::invalidParam);
}

TEST(TamaTest, SmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> sma;

    EXPECT_EQ(tama::sma(prices, sma, 0), status::invalidParam);
    EXPECT_EQ(tama::sma(prices, sma, 3), status::invalidParam);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
