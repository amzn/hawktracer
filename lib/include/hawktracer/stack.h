#ifndef HAWKTRACER_STACK_H
#define HAWKTRACER_STACK_H

#include <hawktracer/bag.h>
#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

typedef struct
{
    HT_Bag sizes_stack;
    void* data;
    size_t size;
    size_t min_capacity;
    size_t capacity;
} HT_Stack;

#define HT_PTR_ADD(ptr, value) (((HT_Byte*)ptr) + value)

void ht_stack_init(HT_Stack* stack, size_t capacity, size_t n_capacity);

void ht_stack_deinit(HT_Stack* stack);

void ht_stack_push(HT_Stack* stack, void* data, size_t size);

void ht_stack_pop(HT_Stack* stack);

#define ht_stack_top(stack) \
    HT_PTR_ADD((stack)->data, (size_t)ht_bag_last((stack)->sizes_stack))

HT_DECLS_END

#endif /* HAWKTRACER_STACK_H */
