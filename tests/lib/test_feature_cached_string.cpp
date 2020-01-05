#include "test_allocator.h"

#include <hawktracer/ht_config.h>
#include <hawktracer/feature_cached_string.h>

#include "test_common.h"
#include <gtest/gtest.h>
#include <unordered_map>

TEST(TestFeatureCachedString, AddMappingShouldEmitStringMappingEvent)
{
    // Arrange
    const char* label = "label";
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_TRUE);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    uintptr_t hash = ht_feature_cached_string_add_mapping(timeline, label);
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ((uintptr_t)label, hash);
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
    ht_feature_cached_string_enable(timeline, HT_FALSE);

    uintptr_t hash_1 = ht_feature_cached_string_add_mapping(timeline, label_1);
    uintptr_t hash_2 = ht_feature_cached_string_add_mapping(timeline, label_2);
    ht_timeline_flush(timeline);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    ht_feature_cached_string_push_map(timeline);
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ((uintptr_t)label_1, hash_1);
    ASSERT_EQ((uintptr_t)label_2, hash_2);
    ASSERT_EQ(2u, string_map_info.values.size());
    std::set<std::string> expected{label_1, label_2};
    std::set<std::string> actual{
        string_map_info.values.front().label,
                string_map_info.values[1].label
    };
    ASSERT_EQ(expected, actual);

    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, EnableFeatureShouldFailIfCantAllocMemory)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);

    ScopedSetAlloc allocator(ht_test_null_realloc);

    // Act
    HT_ErrorCode error_code = ht_feature_cached_string_enable(timeline, HT_TRUE);

    // Assert
    allocator.reset();
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, error_code);
    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, AddMappingShouldFailIfCantAllocMemory)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);

    ScopedSetAlloc allocator(ht_test_null_realloc);
    uintptr_t result = 0;
    int counter = 0;
    std::vector<std::string> labels;

    // Act
    do
    {
        labels.push_back("x");
        result = ht_feature_cached_string_add_mapping(timeline, labels.back().c_str());
        counter++;
    } while (result != 0 && counter < 1024*16 + 1);

    // Assert
    allocator.reset();
    ASSERT_EQ(0u, result);
    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, DynamicStrings)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);
    const char* label = "test_label";
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    uintptr_t hash_1 = ht_feature_cached_string_add_mapping_dynamic(timeline, label);
    uintptr_t hash_2 = ht_feature_cached_string_add_mapping_dynamic(timeline, label);
    ht_timeline_flush(timeline);

    // Assert
    ASSERT_EQ(string_map_info.values.size(), 1u);
    ASSERT_EQ(string_map_info.values.front().identifier, hash_1);
    ASSERT_STREQ(string_map_info.values.front().label, label);
    ASSERT_EQ(hash_1, hash_2);
    ht_timeline_destroy(timeline);
}

TEST(TestFeatureCachedString, PushMapShouldSendDynamicAndStaticMapping)
{
    // Arrange
    NotifyInfo<HT_StringMappingEvent> string_map_info;

    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);
    ht_feature_cached_string_enable(timeline, HT_FALSE);

    std::unordered_map<std::string, uint64_t> labels = {
        {"label1", ht_feature_cached_string_add_mapping(timeline, "label1")},
        {"label2", ht_feature_cached_string_add_mapping_dynamic(timeline, "label2")},
        {"label3", ht_feature_cached_string_add_mapping_dynamic(timeline, "label3")},
        {"label4", ht_feature_cached_string_add_mapping(timeline, "label4")}
    };

    ht_timeline_flush(timeline);

    ht_timeline_register_listener(timeline, test_listener<HT_StringMappingEvent>, &string_map_info);

    // Act
    ht_feature_cached_string_push_map(timeline);
    ht_timeline_flush(timeline);

    // Assert
    std::unordered_map<std::string, uint64_t> actual_labels;
    for (const auto& element : string_map_info.values)
    {
        actual_labels[element.label] = element.identifier;
    }

    ASSERT_EQ(labels, actual_labels);

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
    ht_feature_cached_string_enable(timeline, HT_TRUE);

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
