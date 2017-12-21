#include "test_common.h"
#include "test_test_events.h"

#include <hawktracer/registry.h>

#include <gtest/gtest.h>

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

TEST(TestRegistry, PushAllKlassInfoEventsShouldPushAll)
{
    // Arrange
    size_t event_klass_count = 0;
    HT_EventKlass** klasses = ht_registry_get_event_klasses(&event_klass_count);
    HT_Timeline timeline;
    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, nullptr);
    NotifyInfo<HT_EventKlass> info;
    ht_timeline_register_listener(&timeline, test_listener<HT_EventKlass>, &info);

    // Act
    ht_registry_push_all_klass_info_events(&timeline);

    // Assert
    size_t total_events = 0;
    for (size_t i = 0; i < event_klass_count; i++)
    {
        total_events += klasses[i]->type_info->fields_count + 1;
    }

    ASSERT_EQ(total_events, info.values.size());

    ht_timeline_deinit(&timeline);
}
