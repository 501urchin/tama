#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::wma;
using tama::status;

TEST(TamaTest, WmaMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{0, 0, 12.8333, 15.6667, 14.3333 ,14.5, 13.5 ,14.8333 ,12.5};
    vector<double> wmaOut;

    wma(prices, wmaOut, 3);

    ASSERT_EQ(wmaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(wmaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}



TEST(TamaTest, WmaRejectsEmptyPrices) {
    const vector<double> prices{};
    vector<double> wmaOut{1.0, 2.0};

    const auto result = wma(prices, wmaOut, 3);

    EXPECT_EQ(result, status::emptyPrices);
    EXPECT_EQ(wmaOut.size(), 2u);
}


TEST(TamaTest, WmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> wmaOut;

    EXPECT_EQ(wma(prices, wmaOut, 0), status::invalidParam);
    EXPECT_EQ(wma(prices, wmaOut, 3), status::invalidParam);
}