#include <hawktracer/feature_cached_string.h>

#include "test_common.h"
#include <gtest/gtest.h>

TEST(TestFeatureCachedString, PushingEventTwiceShouldEmitOnlyOneEventToListener)
{
    // Arrange
    const char* label = "label";
    HT_Timeline timeline;
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, NULL);
    ht_feature_cached_string_enable(&timeline);

    ht_timeline_register_listener(&timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    uintptr_t id1 = ht_feature_cached_string_push(&timeline, label);

    // Act
    uintptr_t id2 = ht_feature_cached_string_push(&timeline, label);
    ht_timeline_flush(&timeline);

    // Assert
    ASSERT_EQ(id1, id2);
    ASSERT_EQ(1u, string_map_info.values.size());
    ASSERT_STREQ(label, string_map_info.values.front().label);
    ASSERT_EQ(id1, string_map_info.values.front().hash);

    ht_timeline_deinit(&timeline);
}
