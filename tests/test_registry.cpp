#include <hawktracer/registry.h>

#include <gtest/gtest.h>

TEST(TestRegistry, RegisterNewClassShouldPass)
{
    // Act & Assert
    ASSERT_TRUE(ht_registry_register_timeline("test_RegisterNewClassShouldPass_1", 10, HT_FALSE, nullptr, nullptr));
}

TEST(TestRegistry, RegisterTheSameTimelineSecondTimeShouldSilentlyFail)
{
    // Arrange
    const char* class_name = "test_RegisterTheSameClassSecondTimeShouldSilentlyFail_2";
    ht_registry_register_timeline(class_name, 10, HT_FALSE, nullptr, nullptr);

    // Act & Assert
    ASSERT_FALSE(ht_registry_register_timeline(class_name, 10, HT_FALSE, nullptr, nullptr));
}

HT_DECLARE_EVENT_KLASS(RegistryTestEvent, HT_Event, (INTEGER, int, field))
HT_DEFINE_EVENT_KLASS(RegistryTestEvent, 31338)
size_t ht_RegistryTestEvent_serialize(HT_Event*, HT_Byte*)
{
    return 0;
}

TEST(TestRegistry, RegisterTheSameEventSecondTimeShouldSilentlyFail)
{
    // Arrange
    ht_RegistryTestEvent_register_event_klass();

    // Act & Assert
    ASSERT_FALSE(ht_registry_register_event_klass(ht_RegistryTestEvent_get_event_klass_instance()));
}
