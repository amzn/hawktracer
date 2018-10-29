#include "../../test_allocator.h"

#include <hawktracer/memory_usage.h>

#include <gtest/gtest.h>

TEST(TestMemoryUsage, MemoryUsageShouldReturnPositiveValue)
{
    // Arrange
    HT_MemoryUsageContext* context = ht_memory_usage_context_create(NULL);
    size_t vm_usage = 0, sm_usage = 0, rm_usage = 0;

    // Act
    HT_ErrorCode err = ht_memory_usage_get_usage(context, &vm_usage, &sm_usage, &rm_usage);

    // Assert
    ASSERT_EQ(HT_ERR_OK, err);
    ASSERT_LE(0u, vm_usage);
    ASSERT_LE(0u, sm_usage);
    ASSERT_LE(0u, rm_usage);
    ht_memory_usage_context_destroy(context);
}

TEST(TestMemoryUsage, MemoryUsageOfNegativePIDShouldFailed)
{
    // Arrange
    int pid = -31337;
    HT_MemoryUsageContext* context = ht_memory_usage_context_create(&pid);
    size_t vm_usage = 0;

    // Act
    HT_ErrorCode err = ht_memory_usage_get_usage(context, &vm_usage, NULL, NULL);

    // Assert
    ASSERT_NE(HT_ERR_OK, err);
    ht_memory_usage_context_destroy(context);
}

TEST(TestMemoryUsage, MemoryUsageShouldNotFailIfAllParametersAreNull)
{
    // Arrange
    HT_MemoryUsageContext* context = ht_memory_usage_context_create(NULL);

    // Act
    HT_ErrorCode err = ht_memory_usage_get_usage(context, NULL, NULL, NULL);

    // Assert
    ASSERT_EQ(HT_ERR_OK, err);
    ht_memory_usage_context_destroy(context);
}

TEST(TestMemoryUsage, MemoryUsageCreateShouldFailIfNoMemoryLeft)
{
    // Arrange
    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_MemoryUsageContext* context = ht_memory_usage_context_create(NULL);

    // Assert
    ASSERT_EQ(NULL, context);
}
