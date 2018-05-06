#include <hawktracer/config.h>
#include <hawktracer/feature_cached_string.h>

#include "test_common.h"
#include <gtest/gtest.h>

TEST(TestFeatureCachedString, AddMappingShouldEmitStringMappingEvent)
{
    // Arrange
    const char* label = "label";
    HT_Timeline timeline;
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, NULL);
    ht_feature_cached_string_enable(&timeline);

    ht_timeline_register_listener(&timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    const char* new_label = ht_feature_cached_string_add_mapping(&timeline, label);
    ht_timeline_flush(&timeline);

    // Assert
    ASSERT_EQ(label, new_label);
    ASSERT_EQ(1u, string_map_info.values.size());
    ASSERT_STREQ(label, string_map_info.values.front().label);

    ht_timeline_deinit(&timeline);
}

#ifdef HT_USE_PTHREADS

#include <hawktracer/posix_mapped_tracepoint.h>

TEST(TestFeatureCachedString, PosixMappingShouldAddMappingWhenEnterFunction)
{
    // Arrange
    HT_Timeline timeline;
    NotifyInfo<HT_StringMappingEvent> string_map_info;
    const char* label = "test-label";

    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, NULL);
    ht_feature_cached_string_enable(&timeline);

    ht_timeline_register_listener(&timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    ht_pthread_mapped_tracepoint_enter(&timeline, label);
    ht_pthread_mapped_tracepoint_add_mapping();
    ht_pthread_mapped_tracepoint_leave();
    ht_timeline_flush(&timeline);

    // Assert
    ASSERT_EQ(1u, string_map_info.values.size());
    ASSERT_STREQ(label, string_map_info.values.front().label);

    ht_timeline_deinit(&timeline);
}

#endif
