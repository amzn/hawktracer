#include <hawktracer/stack.h>

#include <gtest/gtest.h>

TEST(TestStack, ShouldNotIncreaseCapacityIfNotNeeded)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, sizeof(int) * 2, 2);

    // Act
    for (int i = 0; i < 2; i++)
    {
        ht_stack_push(&stack, &i, sizeof(int));
    }

    // Assert
    ASSERT_EQ(sizeof(int) * 2, stack.capacity);
    for (int i = 1; i >= 0; i--)
    {
        ASSERT_EQ(i, *(int*)ht_stack_top(&stack));
        ht_stack_pop(&stack);
    }

    ht_stack_deinit(&stack);
}

TEST(TestStack, ShouldIncreaseCapacityIfNeeded)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, sizeof(int) * 8, 2);

    // Act
    for (int i = 0; i < 8; i++)
    {
        ht_stack_push(&stack, &i, sizeof(int));
    }

    // Assert
    ASSERT_GE(sizeof(int) * 8, stack.capacity);
    for (int i = 7; i >= 0; i--)
    {
        ASSERT_EQ(i, *(int*)ht_stack_top(&stack));
        ht_stack_pop(&stack);
    }

    ht_stack_deinit(&stack);
}

TEST(TestStack, ShouldBeAbleToHandleTypesOfSizeBiggerThanMinCapacity)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, 2, 2);

    // Act
    for (long long i = 0; i < 8; i++)
    {
        ht_stack_push(&stack, &i, sizeof(long long));
    }

    // Assert
    ASSERT_GE(sizeof(long long) * 8, stack.capacity);
    for (int i = 7; i >= 0; i--)
    {
        ASSERT_EQ(i, *(long long*)ht_stack_top(&stack));
        ht_stack_pop(&stack);
    }

    ht_stack_deinit(&stack);
}

TEST(TestStack, ShouldNotChangeCapacityAfterEachPopPush)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, sizeof(int) * 2, 2);

    for (int i = 0; i < 4; i++)
    {
        ht_stack_push(&stack, &i, sizeof(int));
    }

    // Act & Assert
    ASSERT_EQ(stack.capacity, 4 * sizeof(int));
    int v = 4;
    ht_stack_push(&stack, &v, sizeof(int));
    ASSERT_EQ(stack.capacity, 8 * sizeof(int));
    ht_stack_pop(&stack);
    ASSERT_EQ(stack.capacity, 8 * sizeof(int));

    ht_stack_deinit(&stack);
}
