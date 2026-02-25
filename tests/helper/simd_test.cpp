#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <numeric>

#include <helpers/helpers.hpp>


TEST(SimdHelpersTest, SimdSumMatchesAccumulate_test) {
    const std::vector<double> values{1.0, -2.5, 3.25, 4.75, -1.5, 0.0, 2.0, 10.0};
    const double expected = std::accumulate(values.begin(), values.end(), 0.0);

    const double result = helpers::simdSum(std::span<const double>(values.data(), values.size()));

    EXPECT_NEAR(result, expected, 1e-12);
}

TEST(SimdHelpersTest, SimdSumHandlesTailElements_test) {
    const std::vector<double> values{0.5, 1.5, 2.5, 3.5, 4.5};
    const double expected = std::accumulate(values.begin(), values.end(), 0.0);

    const double result = helpers::simdSum(std::span<const double>(values.data(), values.size()));

    EXPECT_NEAR(result, expected, 1e-12);
}
