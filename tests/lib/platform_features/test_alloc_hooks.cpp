#include <hawktracer/alloc_hooks.h>

#include <gtest/gtest.h>

#ifdef VALGRIND_CHECK_ENABLED
#include <valgrind/valgrind.h>
#  define CHECK_VALGRIND() do {\
      if (RUNNING_ON_VALGRIND) { \
          printf("[   INFO   ] Test run under valgrind!.The test is actually not executed!\n"); \
          return; \
      } \
  } while (false)
#else /* VALGRIND_CHECK_ENABLED */
#  define CHECK_VALGRIND();
#endif /* VALGRIND_CHECK_ENABLED */

#define VALGRIND_CHECK_ASSERT_EQ(val1, val2) ASSERT_EQ(val1, val2) << "The test can't be run under valgrind. Please disable valgrind and re-run the test.";

struct AllocHooksInfo
{
    void* ret_ptr = NULL;
    void* ptr = NULL;
    size_t size = 0u;
    size_t num = 0u;
    size_t count = 0u;
};

void test_malloc_hook(void* ret, size_t size, void* ud)
{
    AllocHooksInfo* info = static_cast<AllocHooksInfo*>(ud);
    info->ret_ptr = ret;
    info->size = size;
    info->count++;
}

void test_calloc_hook(void* ret, size_t num, size_t size, void* ud)
{
    AllocHooksInfo* info = static_cast<AllocHooksInfo*>(ud);
    info->ret_ptr = ret;
    info->num = num;
    info->size = size;
    info->count++;
}

void test_realloc_hook(void* ret, void* ptr, size_t size, void* ud)
{
    AllocHooksInfo* info = static_cast<AllocHooksInfo*>(ud);
    info->ret_ptr = ret;
    info->ptr = ptr;
    info->size = size;
    info->count++;
}

void test_free_hook(void* ptr, void* ud)
{
    AllocHooksInfo* info = static_cast<AllocHooksInfo*>(ud);
    info->ptr = ptr;
    info->count++;
}

class TestAllocHooks : public ::testing::Test
{
protected:
    void _register_pre_hooks()
    {
        ht_alloc_hooks_register_hooks(test_malloc_hook, NULL, test_calloc_hook, NULL, test_realloc_hook, NULL, test_free_hook, NULL, &_user_data);
    }

    void _register_post_hooks()
    {
        ht_alloc_hooks_register_hooks(NULL, test_malloc_hook, NULL, test_calloc_hook, NULL, test_realloc_hook, NULL, test_free_hook, &_user_data);
    }

    void TearDown() override
    {
        ht_alloc_hooks_register_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    AllocHooksInfo _user_data;
};

TEST_F(TestAllocHooks, TestMallocPreHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_pre_hooks();

    // Act
    void* volatile malloc_ptr = malloc(24);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(NULL, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(24u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(1u, _user_data.count);

    (void)malloc_ptr; // avoid static code analysis tools warnings
    free(malloc_ptr);
}

TEST_F(TestAllocHooks, TestMallocPostHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_post_hooks();

    // Act
    void* malloc_ptr = malloc(24);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(malloc_ptr, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(24u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(1u, _user_data.count);

    free(malloc_ptr);
}

TEST_F(TestAllocHooks, TestCallocPreHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_pre_hooks();

    // Act
    void* volatile calloc_ptr = calloc(16u, 32u);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(NULL, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(16u, _user_data.num);
    VALGRIND_CHECK_ASSERT_EQ(32u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(1u, _user_data.count);

    (void)calloc_ptr; // avoid static code analysis tools warnings
    free(calloc_ptr);
}

TEST_F(TestAllocHooks, TestCallocPostHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_post_hooks();

    // Act
    void* calloc_ptr = calloc(16u, 32u);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(calloc_ptr, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(16u, _user_data.num);
    VALGRIND_CHECK_ASSERT_EQ(32u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(1u, _user_data.count);

    free(calloc_ptr);
}

TEST_F(TestAllocHooks, TestReallocPreHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_pre_hooks();
    void* malloc_ptr = malloc(24);

    // Act
    void* realloc_ptr = realloc(malloc_ptr, 64);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(NULL, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(malloc_ptr, _user_data.ptr);
    VALGRIND_CHECK_ASSERT_EQ(64u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(2u, _user_data.count);

    free(realloc_ptr);
}

TEST_F(TestAllocHooks, TestReallocPostHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_post_hooks();
    void* malloc_ptr = malloc(24);

    // Act
    void* realloc_ptr = realloc(malloc_ptr, 64);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(realloc_ptr, _user_data.ret_ptr);
    VALGRIND_CHECK_ASSERT_EQ(malloc_ptr, _user_data.ptr);
    VALGRIND_CHECK_ASSERT_EQ(64u, _user_data.size);
    VALGRIND_CHECK_ASSERT_EQ(2u, _user_data.count);

    free(realloc_ptr);
}

TEST_F(TestAllocHooks, TestFreePreHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_pre_hooks();
    void* malloc_ptr = malloc(24);

    // Act
    free(malloc_ptr);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(malloc_ptr, _user_data.ptr);
    VALGRIND_CHECK_ASSERT_EQ(2u, _user_data.count);
}

TEST_F(TestAllocHooks, TestFreePostHook)
{
    // Arrange
    CHECK_VALGRIND();
    _register_post_hooks();
    void* malloc_ptr = malloc(24);

    // Act
    free(malloc_ptr);

    // Assert
    VALGRIND_CHECK_ASSERT_EQ(malloc_ptr, _user_data.ptr);
    VALGRIND_CHECK_ASSERT_EQ(2u, _user_data.count);
}

TEST_F(TestAllocHooks, SkipHookFunctionsShouldAllocateMemoryButNotCallCallback)
{
    // Arrange
    CHECK_VALGRIND();
    ht_alloc_hooks_register_hooks(test_malloc_hook, test_malloc_hook,
                                  test_calloc_hook, test_calloc_hook,
                                  test_realloc_hook, test_realloc_hook,
                                  test_free_hook, test_free_hook, &_user_data);

    // Act
    void* volatile malloc_ptr = ht_alloc_hooks_malloc_skip_hook(10);
    void* volatile realloc_ptr = ht_alloc_hooks_realloc_skip_hook(malloc_ptr, 32);
    void* volatile calloc_ptr = ht_alloc_hooks_calloc_skip_hook(2, 4);

    ht_alloc_hooks_free_skip_hook(realloc_ptr);
    ht_alloc_hooks_free_skip_hook(calloc_ptr);

    // Assert
    ASSERT_EQ(0u, _user_data.count);
    ASSERT_NE(nullptr, (void*)malloc_ptr);
    ASSERT_NE(nullptr, (void*)calloc_ptr);
    ASSERT_NE(nullptr, (void*)realloc_ptr);
}
