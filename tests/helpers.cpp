#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <numeric>

#include <tama/helpers.hpp>

TEST(SimdHelpersTest, SimdSumF32MatchesAccumulate) {
    const std::vector<float> values{1.0f, -2.5f, 3.25f, 4.75f, -1.5f, 0.0f, 2.0f};
    const float expected = std::accumulate(values.begin(), values.end(), 0.0f);

    const float result = tama::helpers::simdSumF32(std::span<const float>(values.data(), values.size()));

    EXPECT_NEAR(result, expected, 1e-5f);
}

TEST(SimdHelpersTest, SimdSumF32HandlesEmptySpan) {
    const std::vector<float> values{};

    const float result = tama::helpers::simdSumF32(std::span<const float>(values.data(), values.size()));

    EXPECT_FLOAT_EQ(result, 0.0f);
}

TEST(SimdHelpersTest, SimdSumF64MatchesAccumulate) {
    const std::vector<double> values{1.0, -2.5, 3.25, 4.75, -1.5, 0.0, 2.0, 10.0};
    const double expected = std::accumulate(values.begin(), values.end(), 0.0);

    const double result = tama::helpers::simdSumF64(std::span<const double>(values.data(), values.size()));

    EXPECT_NEAR(result, expected, 1e-12);
}

TEST(SimdHelpersTest, SimdSumF64HandlesTailElements) {
    const std::vector<double> values{0.5, 1.5, 2.5, 3.5, 4.5};
    const double expected = std::accumulate(values.begin(), values.end(), 0.0);

    const double result = tama::helpers::simdSumF64(std::span<const double>(values.data(), values.size()));

    EXPECT_NEAR(result, expected, 1e-12);
}
