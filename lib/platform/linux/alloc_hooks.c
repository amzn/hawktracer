#include "hawktracer/alloc_hooks.h"

#include <stdlib.h>

static HT_AllocHookMalloc _pre_malloc_hook = NULL;
static HT_AllocHookMalloc _post_malloc_hook = NULL;
static HT_AllocHookCalloc _pre_calloc_hook = NULL;
static HT_AllocHookCalloc _post_calloc_hook = NULL;
static HT_AllocHookRealloc _pre_realloc_hook = NULL;
static HT_AllocHookRealloc _post_realloc_hook = NULL;
static HT_AllocHookFree _pre_free_hook = NULL;
static HT_AllocHookFree _post_free_hook = NULL;
static void* _hook_user_data = NULL;

extern void *__libc_malloc(size_t);
extern void *__libc_calloc(size_t, size_t);
extern void __libc_free(void*);
extern void* __libc_realloc(void*, size_t);

void *
malloc(size_t size)
{
    void* ret_ptr;

    if (_pre_malloc_hook)
    {
        _pre_malloc_hook(NULL, size, _hook_user_data);
    }

    ret_ptr = ht_alloc_hooks_malloc_skip_hook(size);

    if (_post_malloc_hook)
    {
        _post_malloc_hook(ret_ptr, size, _hook_user_data);
    }

    return ret_ptr;
}

void *
calloc(size_t num, size_t size)
{
    void* ret_ptr;

    if (_pre_calloc_hook)
    {
        _pre_calloc_hook(NULL, num, size, _hook_user_data);
    }

    ret_ptr = ht_alloc_hooks_calloc_skip_hook(num, size);

    if (_post_calloc_hook)
    {
        _post_calloc_hook(ret_ptr, num, size, _hook_user_data);
    }

    return ret_ptr;
}

void *
realloc(void* ptr, size_t size)
{
    void* ret_ptr;

    if (_pre_realloc_hook)
    {
        _pre_realloc_hook(NULL, ptr, size, _hook_user_data);
    }

    ret_ptr = ht_alloc_hooks_realloc_skip_hook(ptr, size);

    if (_post_realloc_hook)
    {
        _post_realloc_hook(ret_ptr, ptr, size, _hook_user_data);
    }

    return ret_ptr;
}

void
free(void* ptr)
{
    if (_pre_free_hook)
    {
        _pre_free_hook(ptr, _hook_user_data);
    }

    ht_alloc_hooks_free_skip_hook(ptr);

    if (_post_free_hook)
    {
        _post_free_hook(ptr, _hook_user_data);
    }

}

void
ht_alloc_hooks_register_hooks(HT_AllocHookMalloc pre_malloc_hook,
                              HT_AllocHookMalloc post_malloc_hook,
                              HT_AllocHookCalloc pre_calloc_hook,
                              HT_AllocHookCalloc post_calloc_hook,
                              HT_AllocHookRealloc pre_realloc_hook,
                              HT_AllocHookRealloc post_realloc_hook,
                              HT_AllocHookFree pre_free_hook,
                              HT_AllocHookFree post_free_hook,
                              void *user_data)
{
    _pre_malloc_hook = pre_malloc_hook;
    _post_malloc_hook = post_malloc_hook;
    _pre_calloc_hook = pre_calloc_hook;
    _post_calloc_hook = post_calloc_hook;
    _pre_realloc_hook = pre_realloc_hook;
    _post_realloc_hook = post_realloc_hook;
    _pre_free_hook = pre_free_hook;
    _post_free_hook = post_free_hook;
    _hook_user_data = user_data;
}

void *
ht_alloc_hooks_malloc_skip_hook(size_t size)
{
    return __libc_malloc(size);
}

void *
ht_alloc_hooks_calloc_skip_hook(size_t num, size_t size)
{
    return __libc_calloc(num, size);
}

void
ht_alloc_hooks_free_skip_hook(void* ptr)
{
    __libc_free(ptr);
}

void *
ht_alloc_hooks_realloc_skip_hook(void* ptr, size_t size)
{
    return __libc_realloc(ptr, size);
}
