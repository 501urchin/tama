#include <gtest/gtest.h>
#include <vector>

#include <tama/helpers.hpp>

TEST(RingBufferTest, RejectsMismatchedInitialValues) {
    const std::vector<int> values{1, 2};
    EXPECT_THROW(helpers::RingBuffer<int>(3, values), std::invalid_argument);
}

TEST(RingBufferTest, ReadsInitialOrderFromHead) {
    const std::vector<int> values{1, 2, 3};
    helpers::RingBuffer<int> buffer(values.size(), values);

    EXPECT_EQ(buffer.head(), 1);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);
}

TEST(RingBufferTest, InsertAdvancesHeadAndOverwrites) {
    const std::vector<int> values{1, 2, 3};
    helpers::RingBuffer<int> buffer(values.size(), values);

    buffer.insert(4);
    EXPECT_EQ(buffer.head(), 2);
    EXPECT_EQ(buffer[0], 2);
    EXPECT_EQ(buffer[1], 3);
    EXPECT_EQ(buffer[2], 4);

    buffer.insert(5);
    EXPECT_EQ(buffer.head(), 3);
    EXPECT_EQ(buffer[0], 3);
    EXPECT_EQ(buffer[1], 4);
    EXPECT_EQ(buffer[2], 5);
}
