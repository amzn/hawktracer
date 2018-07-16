#include "test_allocator.h"

#include <cstdlib>

void* ht_test_null_realloc(void*, size_t, void*)
{
    return nullptr;
}

void* LimitedSizeAllocator::realloc(void* ptr, size_t size, void* user_data)
{
    if (ptr && !size)
    {
        // It's just a test allocator, we don't care about decreasing
        // current usage.
        free(ptr);
        return nullptr;
    }
    auto alloc_data = reinterpret_cast<LimitedSizeAllocator*>(user_data);
    alloc_data->_current_usage += size;
    if (alloc_data->_current_usage > alloc_data->_max_usage)
    {
        return nullptr;
    }
    return ::realloc(ptr, size);
}

ScopedSetAlloc::ScopedSetAlloc(realloc_function fnc, void *user_data)
{
    ht_allocator_set(fnc, user_data);
}

ScopedSetAlloc::~ScopedSetAlloc()
{
    reset();
}

void ScopedSetAlloc::reset()
{
    ht_allocator_set(nullptr, nullptr);
}
