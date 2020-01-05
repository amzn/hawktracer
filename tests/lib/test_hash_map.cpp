#include "test_allocator.h"

#include <internal/hash_map.h>
#include <unordered_map>
#include <gtest/gtest.h>

TEST(TestHashMap, GetValueShouldReturnNullIfElementDoesNotExist)
{
    // Arrange
    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));

    // Act & Assert
    ASSERT_EQ(NULL, ht_hash_map_get_value(&map, 1234));

    ht_hash_map_deinit(&map);
}

TEST(TestHashMap, InsertShouldAddElementIfNotBeenAddedBefore)
{
    // Arrange
    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));
    HT_ErrorCode err;

    // Act
    const char* ret = ht_hash_map_insert(&map, 1234, "value", &err);

    // Assert
    ASSERT_STREQ("value", ht_hash_map_get_value(&map, 1234));
    ASSERT_EQ(NULL, ret);
    ASSERT_EQ(HT_ERR_OK, err);

    ht_hash_map_deinit(&map);
}

TEST(TestHashMap, InsertStaticShouldUpdateElementIfExists)
{
    // Arrange
    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));
    HT_ErrorCode err;

    // Act
    ht_hash_map_insert(&map, 1234, "value1", NULL);
    const char* ret = ht_hash_map_insert(&map, 1234, "value2", &err);

    // Assert
    ASSERT_STREQ("value2", ht_hash_map_get_value(&map, 1234));
    ASSERT_STREQ("value1", ret);
    ASSERT_EQ(HT_ERR_OK, err);

    ht_hash_map_deinit(&map);
}

TEST(TestHashMap, InitShouldFailIfAllocationFails)
{
    // Arrange
    HT_HashMap map;
    LimitedSizeAllocator alloc_data(1020);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);

    // Act & Assert
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, ht_hash_map_init(&map));
}

TEST(TestHashMap, InsertShouldFailIfAllocationFails)
{
    // Arrange
    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));

    ScopedSetAlloc allocator(ht_test_null_realloc);
    HT_ErrorCode err;
    const char* ret = NULL;
    size_t i = 0;
    // Act

    do
    {
        ret = ht_hash_map_insert(&map, i++, "value", &err);
    } while (i < 1024 * 16 + 1 && err == HT_ERR_OK);

    // Assert
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, err);
    ASSERT_EQ(NULL, ret);

    allocator.reset();
    ht_hash_map_deinit(&map);
}

TEST(TestHashMap, ForEachShouldVisitAllElements)
{
    // Arrange
    using element_map = std::unordered_map<uint64_t, std::string>;
    element_map expected_elements = {
        {1, "test1"},
        {2, "test2"},
        {3, "test3"}
    };
    element_map actual_elements;

    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));

    for (const auto& e : expected_elements)
    {
        ht_hash_map_insert(&map, e.first, e.second.c_str(), NULL);
    }

    auto method = [](uint64_t key, const char* value, void* ud) {
        (*static_cast<element_map*>(ud))[key] = value;
        return HT_TRUE;
    };

    // Act
    ht_hash_map_for_each(&map, method, &actual_elements);

    // Assert
    ASSERT_EQ(expected_elements, actual_elements);

    ht_hash_map_deinit(&map);
}

TEST(TestHashMap, ForEachShouldStopWhenCallbackReturnsFalse)
{
    // Arrange
    std::unordered_map<uint64_t, std::string> elements = {
        {1, "test1"},
        {2, "test2"},
        {3, "test3"}
    };
    struct {
        int counter = 0;
        int visited_before = 0;
    } user_data;

    HT_HashMap map;
    ASSERT_EQ(HT_ERR_OK, ht_hash_map_init(&map));

    for (const auto& e : elements)
    {
        ht_hash_map_insert(&map, e.first, e.second.c_str(), NULL);
    }

    auto method = [](uint64_t key, const char* value, void* ud) {
        HT_UNUSED(value);
        auto& data = *static_cast<decltype(user_data)*>(ud);
        if (key == 3)
        {
            data.visited_before = data.counter;
            return HT_FALSE;
        }
        data.counter++;
        return HT_TRUE;
    };

    // Act
    ht_hash_map_for_each(&map, method, &user_data);

    // Assert
    ASSERT_EQ(user_data.counter, user_data.visited_before);

    ht_hash_map_deinit(&map);
}
