#include <hawktracer/timeline_registry.h>

#include <gtest/gtest.h>

TEST(TestTimelineRegistry, RegisterNewClassShouldPass)
{
    // Act & Assert
    ASSERT_TRUE(ht_timeline_registry_register("test_RegisterNewClassShouldPass_1", 10, HT_FALSE, nullptr, nullptr));
}


TEST(TestTimelineRegistry, RegisterTheSameClassSecondTimeShouldSilentlyFail)
{
    // Arrange
    const char* class_name = "test_RegisterTheSameClassSecondTimeShouldSilentlyFail_2";
    ht_timeline_registry_register(class_name, 10, HT_FALSE, nullptr, nullptr);

    // Act & Assert
    ASSERT_FALSE(ht_timeline_registry_register(class_name, 10, HT_FALSE, nullptr, nullptr));
}
