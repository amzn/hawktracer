#include <hawktracer/duration_conversion.h>
#include <hawktracer/base_types.h>

#include <gtest/gtest.h>

TEST(TestDurationConversion, CheckDifferentUnits)
{
    // Arrange
    HT_DurationNs base_ns = 1234u;

    // Act & Assert
    ASSERT_EQ(1234u, HT_DUR_NS(base_ns));
    ASSERT_EQ(1234000u, HT_DUR_US(base_ns));
    ASSERT_EQ(1234000000u, HT_DUR_MS(base_ns));
    ASSERT_EQ(1234000000000u, HT_DUR_S(base_ns));
}
