#include "test_allocator.h"

#include <hawktracer/ht_config.h>
#include <hawktracer/feature_cached_string.h>

#include "test_common.h"
#include <gtest/gtest.h>

TEST(TestFeatureCachedString, AddMappingShouldEmitStringMappingEvent)
{
    // Arrange
    const char* label = "label";
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    const char* new_label = ht_feature_cached_string_add_mapping(timeline, label);
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ(label, new_label);
    ASSERT_EQ(1u, string_map_info.values.size());
    ASSERT_STREQ(label, string_map_info.values.front().label);

    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, PushMappingShouldEmitStringMappingEventForEachEntry)
{
    // Arrange
    const char* label_1 = "label1";
    const char* label_2 = "label2";
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline);

    const char* new_label_1 = ht_feature_cached_string_add_mapping(timeline, label_1);
    const char* new_label_2 = ht_feature_cached_string_add_mapping(timeline, label_2);
    ht_timeline_flush(timeline);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    ht_feature_cached_string_push_map(timeline);
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ(label_1, new_label_1);
    ASSERT_EQ(label_2, new_label_2);
    ASSERT_EQ(2u, string_map_info.values.size());
    ASSERT_STREQ(label_1, string_map_info.values.front().label);
    ASSERT_STREQ(label_2, string_map_info.values[1].label);

    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, EnableFeatureShouldFailIfCantAllocMemory)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);

    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_ErrorCode error_code = ht_feature_cached_string_enable(timeline);

    // Assert
    allocator.reset();
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, AddMappingShouldFailIfCantAllocMemory)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline);

    ScopedSetAlloc allocator(ht_test_null_realloc);
    const char* result = nullptr;
    int counter = 0;

    // Act
    do
    {
        result = ht_feature_cached_string_add_mapping(timeline, "test");
    } while (result != nullptr && counter < 2048);

    // Assert
    allocator.reset();
    ASSERT_EQ(nullptr, result);
    ht_timeline_destroy(timeline);
}

#ifdef HT_USE_PTHREADS

#include <hawktracer/posix_mapped_tracepoint.h>

TEST(TestFeatureCachedString, PosixMappingShouldAddMappingWhenEnterFunction)
{
    // Arrange
    NotifyInfo<HT_StringMappingEvent> string_map_info;
    const char* label = "test-label";

    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    ht_pthread_mapped_tracepoint_enter(timeline, label);
    ht_pthread_mapped_tracepoint_add_mapping();
    ht_pthread_mapped_tracepoint_leave();
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ(1u, string_map_info.values.size());
    ASSERT_STREQ(label, string_map_info.values.front().label);

    ht_timeline_destroy(timeline);
}

#endif
