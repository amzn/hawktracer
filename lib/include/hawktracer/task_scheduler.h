#ifndef HAWKTRACER_TASK_SCHEDULER_H
#define HAWKTRACER_TASK_SCHEDULER_H

#include <hawktracer/base_types.h>

HT_DECLS_BEGIN

/**
 * A task scheduler's callback.
 * @return #HT_TRUE if the task should be re-scheduled, or #HT_FALSE if the task should be removed
 * from scheduler after executing it.
 */
typedef HT_Boolean(*HT_TaskCallback)(void* user_data);
/** A task scheduler's task identifier. */
typedef int32_t HT_TaskId;
/** An invalid task identifier. */
#define HT_TASK_SCHEDULER_INVALID_TASK_ID ((HT_TaskId)-1)

/**
 * Task scheduling mode.
 */
typedef enum
{
    /** Ignores task execution delays; if the task has been run after the timeout,
     * next task execution will be performed according to a schedule
     * (so it won't be affected by a previous delay). */
    HT_TASK_SCHEDULING_IGNORE_DELAYS,
    /** Takes delays into account, and restarts the timer for the task after its execution;
     * if task was delayed, next task will be executed after a @b period time of the previous
     * execution. */
    HT_TASK_SCHEDULING_RESTART_TIMER
} HT_TaskSchedulingMode;

typedef struct _HT_TaskScheduler HT_TaskScheduler;

/**
 * Creates an instance of the HT_TaskScheduler structure.
 *
 * @param out_err #HT_ERR_OK, if creation completed succesfully; otherwise, appropriate error code.
 *
 * @return task_scheduler a pointer to the scheduler, or NULL if create failed.
 */
HT_API HT_TaskScheduler* ht_task_scheduler_create(HT_ErrorCode* out_err);

/**
 * Destroys task scheduler data structure.
 *
 * @param task_scheduler a pointer to the scheduler.
 */
HT_API void ht_task_scheduler_destroy(HT_TaskScheduler* task_scheduler);

/**
 * Adds a new task to a scheduler.
 * @param task_scheduler a pointer to the scheduler.
 * @param mode a task scheduling mode. @sa HT_TaskSchedulingMode
 * @param period a minimum time (in nanoseconds) between two executions of the task.
 * @param callback a task's function. Can not be NULL.
 * @param user_data a pointer to a data which is passed to a callback when executed.
 *
 * @return a unique identifier of the task if the task is scheduled successfully; otherwise
 * #HT_TASK_SCHEDULER_INVALID_TASK_ID.
 */
HT_API HT_TaskId ht_task_scheduler_schedule_task(HT_TaskScheduler* task_scheduler,
                                                 HT_TaskSchedulingMode mode,
                                                 HT_DurationNs period,
                                                 HT_TaskCallback callback,
                                                 void* user_data);

/**
 * Deletes a task from a scheduler.
 *
 * @param task_scheduler a pointer to the scheduler.
 * @param task_id an identifier of the task to remove.
 *
 * @return #HT_TRUE, if task was removed successfully; if task doesn't exist, returns #HT_FALSE.
 */
HT_API HT_Boolean ht_task_scheduler_remove_task(HT_TaskScheduler* task_scheduler, HT_TaskId task_id);

/**
 * Executes scheduled tasks when they time out.
 *
 * @param task_scheduler a pointer to the scheduler.
 */
HT_API void ht_task_scheduler_tick(HT_TaskScheduler* task_scheduler);

/**
 * Gets optimal tick period for scheduled tasks.
 * The function returns the greatest common divisor of all task periods.
 * E.g. for following periods of scheduled tasks: (10, 15, 25), optimal tick period is 5.
 *
 * @note This function ignores tasks with period equal to 0. Therefore optimal tick period
 * for task periods: (10, 15, 25, 0) is still 5.
 *
 * @param task_scheduler a pointer to the scheduler.
 *
 * @return optimal tick period.
 */
HT_API HT_DurationNs ht_task_scheduler_get_optimal_tick_period(HT_TaskScheduler* task_scheduler);

HT_DECLS_END

#endif /* HAWKTRACER_TASK_SCHEDULER_H */
