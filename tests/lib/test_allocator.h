#ifndef HAWKTRACER_TESTS_TEST_ALLOCATOR_H
#define HAWKTRACER_TESTS_TEST_ALLOCATOR_H

#include <hawktracer/alloc.h>

void* ht_test_null_realloc(void* ptr, size_t size, void* user_data);

class ScopedSetAlloc
{
public:
    ScopedSetAlloc(realloc_function fnc);
    ~ScopedSetAlloc();

    void reset();
};

#endif /* HAWKTRACER_TESTS_TEST_ALLOCATOR_H */
