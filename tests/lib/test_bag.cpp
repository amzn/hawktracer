#include <hawktracer/bag.h>

#include <gtest/gtest.h>

TEST(TestBag, CheckData)
{
    // Arrange
    HT_Bag bag;
    ht_bag_init(&bag, 4);

    // Act
    for (intptr_t i = 0; i < 10; i++)
    {
        ht_bag_add(&bag, (void*)i);
    }

    // Assert
    for (intptr_t i = 0; i < 10; i++)
    {
        ASSERT_EQ(i, (intptr_t)bag.data[i]);
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
    ht_bag_init(&bag, 4);

    // Act
    for (int i = 0; i < 10; i++)
    {
        ht_bag_add(&bag, (void*)1);
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
    ht_bag_init(&bag, 4);

    for (intptr_t i = 0; i < 30; i++)
    {
        ht_bag_add(&bag, (void*)i);
    }

    // Act
    for (intptr_t i = 0; i < 10; i++)
    {
        ht_bag_remove(&bag, (void*)i);
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
    ht_bag_init(&bag, 4);

    for (intptr_t i = 0; i < 30; i++)
    {
        ht_bag_add(&bag, (void*)i);
    }

    // Act
    for (intptr_t i = 0; i < 24; i++)
    {
        ht_bag_remove(&bag, (void*)i);
    }

    // Assert
    ASSERT_EQ(16u, bag.capacity);
    ASSERT_EQ(6u, bag.size);
    ASSERT_EQ(4u, bag.min_capacity);

    ht_bag_deinit(&bag);
}
