#include <hawktracer/alloc.h>

#include <gtest/gtest.h>

struct TestAllocInfo
{
    void* ptr;
    size_t size;
};

static void* test_realloc_function(void* ptr, size_t size, void* user_data)
{
    TestAllocInfo* info = (TestAllocInfo*)user_data;

    info->ptr = ptr;
    info->size = size;

    return nullptr;
}

class TestAlloc : public ::testing::Test
{
protected:
    void TearDown() override
    {
        ht_allocator_set(nullptr, nullptr);
    }
};

TEST_F(TestAlloc, SettingCustomAllocatorShouldChangeAlloc)
{
    // Arrange
    TestAllocInfo info;
    ht_allocator_set(test_realloc_function, &info);

    // Act
    ht_alloc(10);

    // Assert
    ASSERT_EQ(nullptr, info.ptr);
    ASSERT_EQ(10u, info.size);
}

TEST_F(TestAlloc, SettingCustomAllocatorShouldChangeFree)
{
    // Arrange
    TestAllocInfo info;
    ht_allocator_set(test_realloc_function, &info);

    // Act
    ht_free(&info);

    // Assert
    ASSERT_EQ(&info, info.ptr);
    ASSERT_EQ(0u, info.size);
}

TEST_F(TestAlloc, SettingCustomAllocatorShouldChangeRealloc)
{
    // Arrange
    TestAllocInfo info;
    ht_allocator_set(test_realloc_function, &info);

    // Act
    ht_realloc(&info, 20);

    // Assert
    ASSERT_EQ(&info, info.ptr);
    ASSERT_EQ(20u, info.size);
}

TEST_F(TestAlloc, ResettingCustomAllocatorShouldNotFail)
{
    // Arrange
    ht_allocator_set(test_realloc_function, nullptr);

    // Act
    ht_allocator_set(nullptr, nullptr);

    // Assert
    // Following instructions should not fail;
    void* ptr = ht_alloc(30);
    ht_free(ptr);
}
