#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>

using std::vector;
using namespace tama;


TEST(TamaTest, SmaMatchesKnownValues) {
    const vector<double> prices{11, 12, 14, 18, 12, 15, 13, 16, 10};
    const vector<double> expected{0, 0, 12.333, 14.667, 14.667, 15, 13.333, 14.667, 13};
    vector<double> smaOut;

    SimpleMovingAverage smaCalc(3);
    smaCalc.compute(prices, smaOut);

    ASSERT_EQ(smaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++)
    {
        EXPECT_NEAR(smaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, SmaRejectsemptyParams) {
    const vector<double> prices{};
    vector<double> smaOut{1.0, 2.0};
    SimpleMovingAverage smaCalc(3);

    const auto result = smaCalc.compute(prices, smaOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(smaOut.size(), 2u);
}

TEST(TamaTest, SmaRejectsInvalidParams) {
    const vector<double> prices{10, 11, 12};
    vector<double> smaOut;
    SimpleMovingAverage smaCalcInvalid(0);
    SimpleMovingAverage smaCalcEqualSize(3);

    EXPECT_EQ(smaCalcInvalid.compute(prices, smaOut), status::invalidParam);
    EXPECT_EQ(smaCalcEqualSize.compute(prices, smaOut), status::invalidParam);
}

TEST(TamaTest, SmaConstructorUsesPreviousWindowForUpdate) {
    const vector<double> prices{11, 12, 14, 18, 12, 15, 13, 16, 10};
    const size_t period = 3;
    const double newPrice = 19.0;
    vector<double> smaOut;

    SimpleMovingAverage baseline(period);
    ASSERT_EQ(baseline.compute(prices, smaOut), status::ok);

    vector<double> prevWindow(prices.end() - static_cast<std::ptrdiff_t>(period), prices.end());
    SimpleMovingAverage resumed(period, prevWindow);

    const double baselineUpdated = baseline.update(newPrice);
    const double resumedUpdated = resumed.update(newPrice);

    EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}