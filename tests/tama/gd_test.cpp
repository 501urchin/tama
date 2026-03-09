#include <gtest/gtest.h>
#include <tama/tama.hpp>
#include <vector>
#include <stdexcept>

using std::vector;

using tama::GeneralizedDoubleExponentialMovingAverage;

TEST(TamaTest, GdMatchesKnownValues_test) {
	const vector<double> prices{11,12,14,18,12,15,13,16,10};
	const vector<double> expected{11.0,11.675,13.275,16.55625,13.6875,14.5734375,13.5515625,15.183203125,11.7453125};
	vector<double> gdOut;

	GeneralizedDoubleExponentialMovingAverage gd(0.7, 3);
	const auto result = gd.compute(prices, gdOut);

	ASSERT_EQ(result, status::ok);
	ASSERT_EQ(gdOut.size(), prices.size()) << "Vectors differ in size";
	for (size_t i = 0; i < expected.size(); i++) {
		EXPECT_NEAR(gdOut[i], expected[i], 1e-6) << "Vectors differ at index " << i;
	}
}

TEST(TamaTest, GdComputeThenUpdateMatchesKnownValues_test) {
	const vector<double> prices{11,12,14,18,12,15,13,16,10};
	const vector<double> expected{11.0,11.675,13.275,16.55625,13.6875,14.5734375,13.5515625,15.183203125,11.7453125};
	const size_t split = 5;
	vector<double> initialPrices(prices.begin(), prices.begin() + static_cast<std::ptrdiff_t>(split));
	vector<double> mixedOut;

	GeneralizedDoubleExponentialMovingAverage gd(0.7, 3);
	ASSERT_EQ(gd.compute(initialPrices, mixedOut), status::ok);

	for (size_t i = 0; i < split; i++) {
		EXPECT_NEAR(mixedOut[i], expected[i], 1e-6) << "Vectors differ at index " << i;
	}

	for (size_t i = split; i < prices.size(); i++) {
		mixedOut.push_back(gd.update(prices[i]));
	}

	ASSERT_EQ(mixedOut.size(), expected.size());
	for (size_t i = 0; i < expected.size(); i++) {
		EXPECT_NEAR(mixedOut[i], expected[i], 1e-6) << "Vectors differ at index " << i;
	}
}

TEST(TamaTest, GdStateRoundTripMatchesUpdate_test) {
	const vector<double> prices{11,12,14,18,12,15,13,16,10};
	const double newPrice = 19.0;
	vector<double> gdOut;

	GeneralizedDoubleExponentialMovingAverage baseline(0.7, 3);
	ASSERT_EQ(baseline.compute(prices, gdOut), status::ok);

	auto prevState = baseline.getState();
	GeneralizedDoubleExponentialMovingAverage resumed(prevState);

	const double baselineUpdated = baseline.update(newPrice);
	const double resumedUpdated = resumed.update(newPrice);

	EXPECT_NEAR(resumedUpdated, baselineUpdated, 1e-12);
}

TEST(TamaTest, GdRejectsEmptyParams_test) {
	const vector<double> prices{};
	vector<double> gdOut{1.0, 2.0};

	GeneralizedDoubleExponentialMovingAverage gd(0.7, 3);
	const auto result = gd.compute(prices, gdOut);

	EXPECT_EQ(result, status::emptyParams);
	EXPECT_EQ(gdOut.size(), 2u);
}

TEST(TamaTest, GdRejectsInvalidParams_test) {
	EXPECT_THROW(GeneralizedDoubleExponentialMovingAverage(0.0, 3), std::invalid_argument);
	EXPECT_THROW(GeneralizedDoubleExponentialMovingAverage(0.7, 0), std::invalid_argument);
}
