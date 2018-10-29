#include "../../test_allocator.h"

#include <hawktracer/cpu_usage.h>

#include <gtest/gtest.h>

TEST(TestCPUUsage, CPUUsageShouldReturnValueInExpectedRange)
{
    // Arrange
    HT_CPUUsageContext* context = ht_cpu_usage_context_create(NULL);

    // Act
    float usage = ht_cpu_usage_get_percentage(context);

    // Assert
    ASSERT_GE(100.f, usage);
    ASSERT_LE(0.f, usage);
    ht_cpu_usage_context_destroy(context);
}

TEST(TestCPUUsage, CPUUsageOfNegativePIDShouldFailed)
{
    // Arrange
    int pid = -31337;
    HT_CPUUsageContext* context = ht_cpu_usage_context_create(&pid);

    // Act
    float usage = ht_cpu_usage_get_percentage(context);

    // Assert
    ASSERT_GT(0.f, usage);
    ht_cpu_usage_context_destroy(context);
}

TEST(TestCPUUsage, CPUUsageCreateShouldFailIfNoMemoryLeft)
{
    // Arrange
    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_CPUUsageContext* context = ht_cpu_usage_context_create(NULL);

    // Assert
    ASSERT_EQ(NULL, context);
}
