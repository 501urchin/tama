#include <gtest/gtest.h>
#include <vector>

#include <helpers/helpers.hpp>

TEST(RingBufferTest, ReadsInitialOrderFromHead_test) {
    const std::vector<int> values{1, 2, 3};
    helpers::RingBuffer<int> buffer(values.size());

    buffer.insert(values);
    EXPECT_EQ(buffer.head(), 1);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);
}

TEST(RingBufferTest, InsertAdvancesHeadAndOverwrites_test) {
    const std::vector<int> values{1, 2, 3};
    helpers::RingBuffer<int> buffer(values.size());

    buffer.insert(values);
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


TEST(RingBufferTest, RingBufferMax) {
    helpers::RingBuffer<int> buffer(3);

 
    buffer.insert({1, 2, 3});
    EXPECT_EQ(buffer.max(), 3);

    buffer.insert(4);
    EXPECT_EQ(buffer.max(), 4);

    buffer.insert({2, 2, 3});
    EXPECT_EQ(buffer.max(), 3);
}

TEST(RingBufferTest, RingBufferMin) {
    helpers::RingBuffer<int> buffer(3);

    buffer.insert({3, 2, 5});
    EXPECT_EQ(buffer.min(), 2);

    buffer.insert(1);
    EXPECT_EQ(buffer.min(), 1);

    buffer.insert({4, 3, 2});
    EXPECT_EQ(buffer.min(), 2);
}