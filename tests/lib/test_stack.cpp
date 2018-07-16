#include "test_allocator.h"

#include <internal/stack.h>

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

TEST(TestStack, InitShouldFailIfMallocReturnsNull)
{
    // Arrange
    HT_Stack stack;
    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_ErrorCode error_code = ht_stack_init(&stack, 8, 4);

    // Assert
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
}

TEST(TestStack, InitShouldFailIfInternalInitializationFails)
{
    // Arrange
    HT_Stack stack;
    LimitedSizeAllocator alloc_data(16u);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);

    // Act
    HT_ErrorCode error_code = ht_stack_init(&stack, 8, 1024);

    // Assert
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
}

TEST(TestStack, PushShouldFailIfMallocReturnsNull)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, sizeof(int), 1);
    ScopedSetAlloc allocator(ht_test_null_realloc);

    int v = 2;
    HT_ErrorCode error_code_1 = ht_stack_push(&stack, &v, sizeof(v));

    // Act
    HT_ErrorCode error_code_2 = ht_stack_push(&stack, &v, sizeof(v));

    // Assert
    ASSERT_EQ(HT_ERR_OK, error_code_1);
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code_2);
    allocator.reset();
    ht_stack_deinit(&stack);
}

TEST(TestStack, PushShouldFailIfMallocReturnsNullInternal)
{
    // Arrange
    HT_Stack stack;
    ht_stack_init(&stack, sizeof(int), 1);
    LimitedSizeAllocator alloc_data(16u);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);

    // Act
    HT_ErrorCode error_code = HT_ERR_OK;

    do {
        int v = 2;
        error_code = ht_stack_push(&stack, &v, sizeof(v));
    } while (error_code == HT_ERR_OK);

    // Assert
    EXPECT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
    allocator.reset();
    ht_stack_deinit(&stack);
}
