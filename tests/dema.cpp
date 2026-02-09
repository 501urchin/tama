#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::dema;
using tama::status;

TEST(TamaTest, DemaMatchesKnownValues) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> expected{10, 11.5, 11.25, 12.625, 12.3125, 13.6562, 14.8281, 13.6641, 13.957, 15.541};
    vector<double> demaOut;

    dema(prices, demaOut, 3);

    ASSERT_EQ(demaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(demaOut[i], expected[i], 1e-3) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, DemaRejectsEmptyParams) {
    const vector<double> prices{};
    vector<double> demaOut{1.0, 2.0};

    const auto result = dema(prices, demaOut, 3);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(demaOut.size(), 2u);
}

TEST(TamaTest, DemaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> demaOut;

    EXPECT_EQ(dema(prices, demaOut, 0), status::invalidParam);
}
