#include "internal/stack.h"
#include "hawktracer/alloc.h"

#include <string.h>

static HT_Boolean
_ht_stack_resize(HT_Stack* stack, size_t new_capacity)
{
    void* new_data = ht_realloc(stack->data, new_capacity);

    if (new_data == NULL)
    {
        return HT_FALSE;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;

    return HT_TRUE;
}

HT_ErrorCode ht_stack_init(HT_Stack* stack, size_t capacity, size_t n_capacity)
{
    HT_ErrorCode bag_init_err = HT_ERR_OK;

    stack->data = ht_alloc(capacity);

    if (stack->data == NULL)
    {
        return HT_ERR_OUT_OF_MEMORY;
    }

    bag_init_err = ht_bag_init(&stack->sizes_stack, n_capacity);
    if (bag_init_err != HT_ERR_OK)
    {
        ht_free(stack->data);
        return bag_init_err;
    }

    stack->size = 0;
    stack->capacity = capacity;
    stack->min_capacity = capacity;

    return HT_ERR_OK;
}

void
ht_stack_deinit(HT_Stack* stack)
{
    ht_bag_deinit(&stack->sizes_stack);
    stack->min_capacity = 0;
    stack->size = stack->capacity = stack->min_capacity = 0;
    ht_free(stack->data);
}

HT_ErrorCode
ht_stack_push(HT_Stack* stack, void* data, size_t size)
{
    HT_ErrorCode bag_err_code = HT_ERR_OK;

    if (stack->capacity - stack->size < size)
    {
        size_t factor = 2;
        while (stack->capacity * factor - stack->size < size)
        {
            factor *= 2;
        }
        if (!_ht_stack_resize(stack, stack->capacity * factor))
        {
            return HT_ERR_OUT_OF_MEMORY;
        }
    }

    bag_err_code = ht_bag_add(&stack->sizes_stack, (void*)stack->size);
    if (bag_err_code != HT_ERR_OK)
    {
        return bag_err_code;
    }

    memcpy(HT_PTR_ADD(stack->data, stack->size), data, size);
    stack->size += size;

    return HT_ERR_OK;
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
