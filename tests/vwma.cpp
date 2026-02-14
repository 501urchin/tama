#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::vwma;


TEST(TamaTest, VwmaMatchesKnownValues) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> volume{100, 120, 80, 150, 130, 110, 90, 140, 100, 120};
    const vector<double> expected{0, 0, 11.07, 12.20, 12.19, 12.95, 13.48, 13.85, 13.85, 14.28};
    vector<double> vwmaOut;

    vwma(prices, volume, vwmaOut, 3);

    ASSERT_EQ(vwmaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(vwmaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}



TEST(TamaTest, VwmaRejectsemptyParams) {
    const vector<double> prices{};
    const vector<double> volume{};
    vector<double> vwmaOut{1.0, 2.0};

    const auto result = vwma(prices, volume, vwmaOut, 3);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(vwmaOut.size(), 2u);
}


TEST(TamaTest, VwmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    const vector<double> volume{100, 110, 120};
    vector<double> vwmaOut;

    EXPECT_EQ(vwma(prices, volume, vwmaOut, 0), status::invalidParam);
    EXPECT_EQ(vwma(prices, volume, vwmaOut, 3), status::invalidParam);
}


TEST(TamaTest, VwmaRejectsMismatchedLengths) {
    const vector<double> prices{10, 11, 12, 13};
    const vector<double> volume{100, 110, 120};
    vector<double> vwmaOut;

    EXPECT_EQ(vwma(prices, volume, vwmaOut, 3), status::invalidParam);
}
