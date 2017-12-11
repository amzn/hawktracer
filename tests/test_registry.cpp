#include <hawktracer/registry.h>

#include <gtest/gtest.h>

#include "test_test_events.h"


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

TEST(TestRegistry, RegisterTheSameEventSecondTimeShouldSilentlyFail)
{
    // Arrange
    ht_RegistryTestEvent_register_event_klass();

    // Act & Assert
    ASSERT_FALSE(ht_registry_register_event_klass(ht_RegistryTestEvent_get_event_klass_instance()));
}

TEST(TestRegistry, RegisterEventTwiceShouldAssignDifferentIdentifiersToTheClass)
{
    // Arrange

    // Act
    ht_RegistryTestEvent_ID_ONE_register_event_klass();
    ht_RegistryTestEvent_ID_TWO_register_event_klass();

    // Act & Assert
    HT_EventId id_one = ht_RegistryTestEvent_ID_ONE_get_event_klass_instance()->type;
    HT_EventId id_two = ht_RegistryTestEvent_ID_TWO_get_event_klass_instance()->type;

    ASSERT_NE(id_one, id_two);
}
