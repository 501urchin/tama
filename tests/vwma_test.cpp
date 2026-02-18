#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <stdexcept>
#include <vector>

using std::vector;

TEST(TamaTest, VwmaMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> volume{100, 120, 80, 150, 130, 110, 90, 140, 100, 120};
    const vector<double> expected{0, 0, 11.07, 12.20, 12.19, 12.95, 13.48, 13.85, 13.85, 14.28};
    vector<double> vwmaOut;
    tama::VolumeWeightedMovingAverage vwma(3);

    vwma.compute(prices, volume, vwmaOut);

    ASSERT_EQ(vwmaOut.size(), prices.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(vwmaOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}



TEST(TamaTest, VwmaRejectsemptyParams_test) {
    const vector<double> prices{};
    const vector<double> volume{};
    vector<double> vwmaOut{1.0, 2.0};
    tama::VolumeWeightedMovingAverage vwma(3);

    const auto result = vwma.compute(prices, volume, vwmaOut);

    EXPECT_EQ(result, status::emptyParams);
    EXPECT_EQ(vwmaOut.size(), 2u);
}


TEST(TamaTest, VwmaRejectsInvalidParams_test) {
    const vector<double> prices{10, 11, 12};
    const vector<double> volume{100, 110, 120};
    vector<double> vwmaOut;

    EXPECT_THROW(tama::VolumeWeightedMovingAverage(0), std::invalid_argument);

    tama::VolumeWeightedMovingAverage vwma(3);
    EXPECT_EQ(vwma.compute(prices, volume, vwmaOut), status::invalidParam);
}


TEST(TamaTest, VwmaRejectsMismatchedLengths_test) {
    const vector<double> prices{10, 11, 12, 13};
    const vector<double> volume{100, 110, 120};
    vector<double> vwmaOut;
    tama::VolumeWeightedMovingAverage vwma(3);

    EXPECT_EQ(vwma.compute(prices, volume, vwmaOut), status::invalidParam);
}

TEST(TamaTest, VwmaComputeThenUpdateMatchesKnownValues_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> volume{100, 120, 80, 150, 130, 110, 90, 140, 100, 120};
    const vector<double> expected{0, 0, 11.07, 12.20, 12.19, 12.95, 13.48, 13.85, 13.85, 14.28};
    const size_t period = 3;
    const size_t split = 6;

    vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> initialVolume(volume.begin(), volume.begin() + static_cast<std::ptrdiff_t>(split));
    vector<double> mixedOut;

    tama::VolumeWeightedMovingAverage vwma(period);
    ASSERT_EQ(vwma.compute(initialPrices, initialVolume, mixedOut), status::ok);

    for (size_t i = 0; i < split; i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }

    for (size_t i = split; i < prices.size(); i++) {
        mixedOut.push_back(vwma.update(prices[i], volume[i]));
    }

    ASSERT_EQ(mixedOut.size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_NEAR(mixedOut[i], expected[i], 1e-1) << "Vectors differ at index " << i;
    }
}

TEST(TamaTest, VwmaConstructorUsesPreviousWindowForUpdate_test) {
    const vector<double> prices{10, 12, 11, 13, 12, 14, 15, 13, 14, 16};
    const vector<double> volume{100, 120, 80, 150, 130, 110, 90, 140, 100, 120};
    const size_t period = 3;
    const double newPrice = 17.0;
    const double newVolume = 130.0;
    vector<double> vwmaOut;

    tama::VolumeWeightedMovingAverage baseline(period);
    ASSERT_EQ(baseline.compute(prices, volume, vwmaOut), status::ok);

    vector<double> prevPrices(prices.end() - static_cast<std::ptrdiff_t>(period), prices.end());
    vector<double> prevVolume(volume.end() - static_cast<std::ptrdiff_t>(period), volume.end());
    tama::VolumeWeightedMovingAverage resumed(period, prevPrices, prevVolume);

    const double baselineUpdated = baseline.update(newPrice, newVolume);
    const double resumedUpdated = resumed.update(newPrice, newVolume);

    EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}
