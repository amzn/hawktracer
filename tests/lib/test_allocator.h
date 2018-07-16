#ifndef HAWKTRACER_TESTS_TEST_ALLOCATOR_H
#define HAWKTRACER_TESTS_TEST_ALLOCATOR_H

#include <hawktracer/alloc.h>

struct LimitedSizeAllocator
{
public:
    LimitedSizeAllocator(size_t max_usage) :
        _max_usage(max_usage)
    {}

    static void* realloc(void* ptr, size_t size, void* user_data);

private:
    const size_t _max_usage;
    size_t _current_usage = 0;
};

void* ht_test_null_realloc(void* ptr, size_t size, void* user_data);

class ScopedSetAlloc
{
public:
    ScopedSetAlloc(realloc_function fnc, void* user_data = nullptr);
    ~ScopedSetAlloc();

    void reset();
};

#endif /* HAWKTRACER_TESTS_TEST_ALLOCATOR_H */
