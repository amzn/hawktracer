#include "test_common.h"
#include "test_test_events.h"

#include <hawktracer/registry.h>
#include <internal/registry.h>

#include <gtest/gtest.h>

TEST(TestRegistry, RegisterTheSameEventSecondTimeShouldReturnTheSameType)
{
    // Arrange
    uint32_t type = HT_REGISTER_EVENT_KLASS(RegistryTestEvent);

    // Act & Assert
    ASSERT_EQ(type, ht_registry_register_event_klass(ht_RegistryTestEvent_get_event_klass_instance()));
}

TEST(TestRegistry, RegisterEventTwiceShouldAssignDifferentIdentifiersToTheClass)
{
    // Arrange

    // Act
    HT_REGISTER_EVENT_KLASS(RegistryTestEvent_ID_ONE);
    HT_REGISTER_EVENT_KLASS(RegistryTestEvent_ID_TWO);

    // Act & Assert
    HT_EventId id_one = ht_RegistryTestEvent_ID_ONE_get_event_klass_instance()->klass_id;
    HT_EventId id_two = ht_RegistryTestEvent_ID_TWO_get_event_klass_instance()->klass_id;

    ASSERT_NE(id_one, id_two);
}

TEST(TestRegistry, PushAllKlassInfoEventsShouldPushAll)
{
    // Arrange
    size_t event_klass_count = 0;
    HT_EventKlass** klasses = ht_registry_get_event_klasses(&event_klass_count);
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, nullptr, nullptr);
    NotifyInfo<HT_EventKlass> info;

    // Act
    ht_registry_push_registry_klasses_to_listener(test_listener<HT_EventKlass>, &info, HT_FALSE);

    // Assert
    size_t total_events = 0;
    for (size_t i = 0; i < event_klass_count; i++)
    {
        total_events += klasses[i]->type_info->fields_count + 1;
    }

    ASSERT_EQ(total_events, info.values.size());

    ht_timeline_destroy(timeline);
}

TEST(TestRegistry, RegisterListenerTwiceShouldFail)
{
    // Arrange
    const char* container_name = "conatiner_name_RegisterListenerTwiceShouldFail";
    HT_TimelineListenerContainer* container = ht_timeline_listener_container_create();
    ASSERT_EQ(HT_ERR_OK, ht_registry_register_listener_container(container_name, container));

    // Act & Assert
    HT_TimelineListenerContainer* container2 = ht_timeline_listener_container_create();
    ASSERT_EQ(HT_ERR_LISTENER_CONTAINER_ALREADY_REGISTERED, ht_registry_register_listener_container(container_name, container2));
    ht_timeline_listener_container_unref(container2);
    ht_timeline_listener_container_unref(container);
}

TEST(TestRegistry, RegisterTheSameFeatureTwiceShouldFail)
{
    // Arrange
    uint32_t feature_id = HT_TIMELINE_MAX_FEATURES - 1;

    // Act & Assert
    ASSERT_EQ(HT_ERR_OK, ht_registry_register_feature(feature_id, (HT_FeatureDisableCallback)1));
    ASSERT_EQ(HT_ERR_FEATURE_ALREADY_REGISTERED, ht_registry_register_feature(feature_id, (HT_FeatureDisableCallback)1));
}
