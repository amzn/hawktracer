#include <hawktracer/CircularBuffer.h>

#include <gtest/gtest.h>

using hawktracer::CircularBuffer;

TEST(CircularBufferTest, IsEmptyAtTheBeginning)
{
    CircularBuffer<int, 3> buffer;

    ASSERT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, IsEmptyAfterRemovingAllItems)
{
    CircularBuffer<int, 3> buffer;

    buffer.push(1);
    buffer.push(2);

    buffer.pop();
    buffer.pop();

    ASSERT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, ShouldOverrideTopElementWhenOverflow)
{
    CircularBuffer<int, 3> buffer;

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);
    buffer.push(5);

    ASSERT_EQ(3, buffer.front());
}

TEST(CircularBufferTest, ShouldBeEmptyWhenOverflowAndRemoveAllElements)
{
    CircularBuffer<int, 3> buffer;

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);
    buffer.push(5);

    buffer.pop();
    buffer.pop();
    buffer.pop();

    ASSERT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, PushAndPopMix_1)
{
    CircularBuffer<int, 3> buffer;

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.pop();
    buffer.push(4);
    buffer.push(5);
    buffer.pop();

    ASSERT_EQ(4, buffer.front());
}

TEST(CircularBufferTest, PushAndPopMix_2)
{
    CircularBuffer<int, 3> buffer;

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);
    buffer.pop();
    buffer.pop();
    buffer.push(6);
    buffer.push(7);
    buffer.pop();
    buffer.pop();

    ASSERT_EQ(7, buffer.front());
}
