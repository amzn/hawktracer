#include "test_allocator.h"

#include <internal/bag.h>

#include <gtest/gtest.h>

TEST(TestBag, CheckData)
{
    // Arrange
    HT_Bag bag;
    ht_bag_init(&bag, 4, sizeof(void*));

    // Act
    for (intptr_t i = 0; i < 10; i++)
    {
        ht_bag_add(&bag, (void*)&i);
    }

    // Assert
    for (intptr_t i = 0; i < 10; i++)
    {
        ASSERT_EQ(i, *(intptr_t*)ht_bag_get_n(&bag, i));
    }

    ASSERT_EQ(16u, bag.capacity);
    ASSERT_EQ(10u, bag.size);
    ASSERT_EQ(4u, bag.min_capacity);

    ht_bag_deinit(&bag);
}

TEST(TestBag, ShouldResizeIfTooManyItems)
{
    // Arrange
    HT_Bag bag;
    ht_bag_init(&bag, 4, sizeof(void*));

    // Act
    for (int i = 0; i < 10; i++)
    {
        ht_bag_add(&bag, (void*)&i);
    }

    // Assert
    ASSERT_EQ(16u, bag.capacity);
    ASSERT_EQ(10u, bag.size);
    ASSERT_EQ(4u, bag.min_capacity);

    ht_bag_deinit(&bag);
}

TEST(TestBag, ShouldNotResizeIfRemoveOnlyFewItems)
{
    // Arrange
    HT_Bag bag;
    ht_bag_init(&bag, 4, sizeof(void*));

    for (intptr_t i = 0; i < 30; i++)
    {
        ht_bag_add(&bag, (void*)&i);
    }

    // Act
    for (intptr_t i = 0; i < 10; i++)
    {
        ht_bag_remove(&bag, (void*)&i);
    }

    // Assert
    ASSERT_EQ(32u, bag.capacity);
    ASSERT_EQ(20u, bag.size);
    ASSERT_EQ(4u, bag.min_capacity);

    ht_bag_deinit(&bag);
}

TEST(TestBag, ShouldResizeIfRemoveManyItems)
{
    // Arrange
    HT_Bag bag;
    ht_bag_init(&bag, 4, sizeof(void*));

    for (intptr_t i = 0; i < 30; i++)
    {
        ht_bag_add(&bag, (void*)&i);
    }

    // Act
    for (intptr_t i = 0; i < 24; i++)
    {
        ht_bag_remove(&bag, (void*)&i);
    }

    // Assert
    ASSERT_EQ(16u, bag.capacity);
    ASSERT_EQ(6u, bag.size);
    ASSERT_EQ(4u, bag.min_capacity);

    ht_bag_deinit(&bag);
}

TEST(TestBag, InitShouldFailIfMallocReturnsNull)
{
    // Arrange
    HT_Bag bag;
    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_ErrorCode error_code = ht_bag_init(&bag, 8, sizeof(void*));

    // Assert
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
}
