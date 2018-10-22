#include "hawktracer/task_scheduler.h"
#include "hawktracer/alloc.h"
#include "hawktracer/monotonic_clock.h"
#include "internal/bag.h"

#include <string.h>

#define _DEFAULT_INIT_TASK_COUNT 16

struct _HT_TaskScheduler
{
    HT_Bag tasks;
    HT_TaskId next_task_id;
};

struct _HT_Task
{
    HT_TaskCallback callback;
    void* user_data;
    HT_DurationNs period;
    HT_TimestampNs next_action_ts;
    HT_TaskId id;
    HT_TaskSchedulingMode mode;
};

#define HT_TASK(task) (*(HT_Task**)task)

static HT_DurationNs
_greatest_common_divisor(HT_DurationNs a, HT_DurationNs b)
{
    while (b)
    {
        HT_DurationNs t = b;
        b =a % b;
        a = t;
    }

    return a;
}

HT_TaskScheduler*
ht_task_scheduler_create(HT_ErrorCode* out_err)
{
    HT_TaskScheduler* task_scheduler = HT_CREATE_TYPE(HT_TaskScheduler);
    HT_ErrorCode error_code;

    if (task_scheduler == NULL)
    {
        error_code = HT_ERR_OUT_OF_MEMORY;
        goto done;
    }

    error_code = ht_bag_init(&task_scheduler->tasks, _DEFAULT_INIT_TASK_COUNT, sizeof(HT_Task*));
    if (error_code != HT_ERR_OK)
    {
        ht_free(task_scheduler);
        task_scheduler = NULL;
        goto done;
    }

    task_scheduler->next_task_id = 0;

done:
    if (out_err != NULL)
    {
        *out_err = error_code;
    }

    return task_scheduler;
}

void
ht_task_scheduler_destroy(HT_TaskScheduler* task_scheduler)
{
    size_t i = 0;
    for (i = 0; i < task_scheduler->tasks.size; i++)
    {
        ht_free(*(HT_Task**)ht_bag_get_n(&task_scheduler->tasks, i));
    }

    ht_bag_deinit(&task_scheduler->tasks);
    ht_free(task_scheduler);
}

HT_TaskId
ht_task_scheduler_schedule_task(HT_TaskScheduler* task_scheduler,
                                HT_TaskSchedulingMode mode,
                                HT_DurationNs period,
                                HT_TaskCallback callback,
                                void* user_data)
{
    HT_Task* task;

    if (task_scheduler->next_task_id == INT32_MAX || callback == NULL)
    {
        return HT_TASK_SCHEDULER_INVALID_TASK_ID;
    }

    task = ht_alloc(sizeof(HT_Task));
    if (task == NULL)
    {
        return HT_TASK_SCHEDULER_INVALID_TASK_ID;
    }

    task->callback = callback;
    task->user_data = user_data;
    task->period = period;
    task->next_action_ts = ht_monotonic_clock_get_timestamp() + period;
    task->id = task_scheduler->next_task_id++;
    task->mode = mode;

    if (ht_bag_add(&task_scheduler->tasks, &task) != HT_ERR_OK)
    {
        return HT_TASK_SCHEDULER_INVALID_TASK_ID;
    }

    return task->id;
}

void
ht_task_scheduler_tick(HT_TaskScheduler* task_scheduler)
{
    size_t i;

    for (i = 0; i < task_scheduler->tasks.size; i++)
    {
        HT_Task* task = HT_TASK(ht_bag_get_n(&task_scheduler->tasks, i));
        HT_TimestampNs now_ts = ht_monotonic_clock_get_timestamp();
        if (task->next_action_ts > now_ts)
        {
            continue;
        }

        task->callback(task->user_data);
        task->next_action_ts = task->mode == HT_TASK_SCHEDULING_IGNORE_DELAYS ?
                    task->next_action_ts + task->period :
                    ht_monotonic_clock_get_timestamp() + task->period;
    }
}

HT_Boolean
ht_task_scheduler_remove_task(HT_TaskScheduler* task_scheduler, HT_TaskId task_id)
{
    size_t i;

    if (task_id < 0)
    {
        return HT_FALSE;
    }

    for (i = 0; i < task_scheduler->tasks.size; i++)
    {
        HT_Task* task = HT_TASK(ht_bag_get_n(&task_scheduler->tasks, i));
        if (task->id == task_id)
        {
            ht_bag_remove(&task_scheduler->tasks, &task);
            ht_free(task);
            return HT_TRUE;
        }
    }

    return HT_FALSE;
}

HT_DurationNs
ht_task_scheduler_get_optimal_tick_period(HT_TaskScheduler* task_scheduler)
{
    size_t task_pos;
    HT_DurationNs perfect_period = 0;

    for (task_pos = 0; task_pos < task_scheduler->tasks.size; task_pos++)
    {
        HT_DurationNs task_period = HT_TASK(ht_bag_get_n(&task_scheduler->tasks, task_pos))->period;

        if (task_period == 0)
        {
            continue;
        }

        perfect_period = perfect_period == 0 ? task_period : _greatest_common_divisor(perfect_period, task_period);
    }

    return perfect_period;
}
