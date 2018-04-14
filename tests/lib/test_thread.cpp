#include <hawktracer/thread.h>

#include <gtest/gtest.h>

#include <thread>

TEST(TestThread, ShouldReturnTheSameIdForTheSameThread)
{
    // Arrange & Act
    HT_ThreadId id1 = ht_thread_get_current_thread_id();
    HT_ThreadId id2 = ht_thread_get_current_thread_id();

    // Assert
    ASSERT_EQ(id1, id2);
}

TEST(TestThread, ShouldReturnDifferentIdForDifferentThreads)
{
    // Arrange & Act
    HT_ThreadId id1 = ht_thread_get_current_thread_id();
    HT_ThreadId id2 = id1;

    std::thread([&id2] {
        id2 = ht_thread_get_current_thread_id();
    }).join();

    // Assert
    ASSERT_NE(id1, id2);
}

TEST(TestThread, ShouldNotRepeatThreadIdForNewThreads)
{
    // Arrange
    HT_ThreadId id1 = 0;
    HT_ThreadId id2 = 0;

    // Act
    std::thread([&id1] {
        id1 = ht_thread_get_current_thread_id();
    }).join();

    std::thread([&id2] {
        id2 = ht_thread_get_current_thread_id();
    }).join();

    // Assert
    ASSERT_NE(id1, id2);
}
