#include "test_allocator.h"

void* ht_test_null_realloc(void*, size_t, void*)
{
    return nullptr;
}

ScopedSetAlloc::ScopedSetAlloc(realloc_function fnc)
{
    ht_allocator_set(fnc, nullptr);
}

ScopedSetAlloc::~ScopedSetAlloc()
{
    reset();
}

void ScopedSetAlloc::reset()
{
    ht_allocator_set(nullptr, nullptr);
}
