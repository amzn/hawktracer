#include <hawktracer/alloc.h>

#include <stdlib.h>

static realloc_function realloc_fnc_ = NULL;
static void* user_data_ = NULL;

void ht_allocator_set(realloc_function func, void* user_data)
{
    realloc_fnc_ = func;
    user_data_ = user_data;
}

void* ht_alloc(size_t size)
{
    /* TODO: check whether if (fnc == NULL) is faster than defining default allocator */
    return (realloc_fnc_ == NULL) ? malloc(size) : realloc_fnc_(NULL, size, user_data_);
}

void* ht_realloc(void* ptr, size_t size)
{
    return (realloc_fnc_ == NULL) ? realloc(ptr, size) : realloc_fnc_(ptr, size, user_data_);
}

void ht_free(void* ptr)
{
    if (realloc_fnc_ == NULL)
    {
        free(ptr);
    }
    else
    {
        realloc_fnc_(ptr, 0, user_data_);
    }
}
