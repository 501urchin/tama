#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;

using tama::md;
using tama::status;

TEST(TamaTest, MdMatchesKnownValues) {
    const vector<double> prices{11,12,14,18,12,15,13,16,10};
    const vector<double> expected{11.0000, 11.0708, 11.1851, 11.2866, 11.3425, 11.4621, 11.5551, 11.6758, 11.3634};
    vector<double> mdOut;

    const auto result = md(prices, mdOut, 10);

    EXPECT_EQ(result, status::ok);
    ASSERT_EQ(mdOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mdOut[i], expected[i], 1e-2) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, MdRejectsEmptyParams) {
    const vector<double> prices{};
    vector<double> mdOut{1.0, 2.0};

    const auto result = md(prices, mdOut, 3);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(mdOut.size(), 2u);
}

TEST(TamaTest, MdRejectsInvalidParams) {
    const vector<double> prices{10.0, 11.0, 12.0};
    vector<double> mdOut;

    const auto result = md(prices, mdOut, 0);

    EXPECT_EQ(result, status::invalidParam);
}
