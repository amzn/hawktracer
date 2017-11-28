#include "hawktracer/stack.h"
#include "hawktracer/alloc.h"

#include <string.h>

static void
_ht_stack_resize(HT_Stack* stack, size_t new_capacity)
{
    stack->capacity = new_capacity;
    stack->data = ht_realloc(stack->data, stack->capacity);
}

void
ht_stack_init(HT_Stack* stack, size_t capacity, size_t n_capacity)
{
    stack->data = ht_alloc(capacity);
    stack->size = 0;
    stack->capacity = capacity;
    stack->min_capacity = capacity;
    ht_bag_init(&stack->sizes_stack, n_capacity);
}

void
ht_stack_deinit(HT_Stack* stack)
{
    ht_bag_deinit(&stack->sizes_stack);
    stack->min_capacity = 0;
    stack->size = stack->capacity = stack->min_capacity = 0;
    ht_free(stack->data);
}

void
ht_stack_push(HT_Stack* stack, void* data, size_t size)
{
    if (stack->capacity - stack->size < size)
    {
        size_t factor = 2;
        while (stack->capacity * factor - stack->size < size)
        {
            factor *= 2;
        }
        _ht_stack_resize(stack, stack->capacity * factor);
    }

    ht_bag_add(&stack->sizes_stack, (void*)stack->size);
    memcpy(HT_PTR_ADD(stack->data, stack->size), data, size);
    stack->size += size;
}

void
ht_stack_pop(HT_Stack* stack)
{
    stack->size = (size_t)ht_bag_last(stack->sizes_stack);
    ht_bag_remove_nth(&stack->sizes_stack, stack->sizes_stack.size - 1);

    if (stack->capacity > stack->min_capacity && stack->size < stack->capacity / 4)
    {
        _ht_stack_resize(stack, stack->capacity / 2);
    }
}
