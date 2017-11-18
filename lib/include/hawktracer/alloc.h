#ifndef HAWKTRACER_ALLOC_H
#define HAWKTRACER_ALLOC_H

#include <hawktracer/macros.h>

#include <stddef.h>

HT_DECLS_BEGIN

typedef void* (*realloc_function)(void* /* ptr */, size_t /* size */, void* /* user_data */);

void ht_allocator_set(realloc_function func, void* user_data);

void* ht_alloc(size_t size);

void* ht_realloc(void* ptr, size_t size);

void ht_free(void* ptr);

#define HT_CREATE_TYPE(Type) (Type*)ht_alloc(sizeof(Type))

HT_DECLS_END

#endif /* HAWKTRACER_ALLOC_H */
