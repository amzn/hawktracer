#include <hawktracer/registry.h>

#include <gtest/gtest.h>

#define HT_DEFINE_TEST_EVENT_KLASS(TYPE_NAME) \
    HT_DECLARE_EVENT_KLASS(TYPE_NAME, HT_Event, (INTEGER, int, field)) \
    HT_DEFINE_EVENT_KLASS(TYPE_NAME) \
    size_t ht_##TYPE_NAME##_serialize(HT_Event*, HT_Byte*) { return 0; }


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

HT_DEFINE_TEST_EVENT_KLASS(RegistryTestEvent)

TEST(TestRegistry, RegisterTheSameEventSecondTimeShouldSilentlyFail)
{
    // Arrange
    ht_RegistryTestEvent_register_event_klass();

    // Act & Assert
    ASSERT_FALSE(ht_registry_register_event_klass(ht_RegistryTestEvent_get_event_klass_instance()));
}

HT_DEFINE_TEST_EVENT_KLASS(RegistryTestEvent_ID_ONE)
HT_DEFINE_TEST_EVENT_KLASS(RegistryTestEvent_ID_TWO)

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
