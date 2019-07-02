#include <hawktracer/task_scheduler.h>
#include <hawktracer/monotonic_clock.h>

#include "test_allocator.h"

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

static HT_Boolean test_callback(void* ud)
{
    int* value = static_cast<int*>(ud);
    (*value)++;
    return HT_TRUE;
}

static HT_Boolean test_callback_false(void* ud)
{
    int* value = static_cast<int*>(ud);
    (*value)++;
    return HT_FALSE;
}

class TestTaskScheduler : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _scheduler = ht_task_scheduler_create(NULL);
    }

    void TearDown() override
    {
        ht_task_scheduler_destroy(_scheduler);
    }

    HT_TaskScheduler* _scheduler;
};

TEST_F(TestTaskScheduler, RegisterNullCallbackShouldFail)
{
    // Arrange
    // Act
    HT_TaskId task_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, NULL, NULL);

    // Assert
    ASSERT_EQ(HT_TASK_SCHEDULER_INVALID_TASK_ID, task_id);
}

TEST_F(TestTaskScheduler, CreateShouldFailIfMallocReturnsNull)
{
    // Arrange
    ScopedSetAlloc allocator(ht_test_null_realloc);
    HT_ErrorCode err;

    // Act
    HT_TaskScheduler* scheduler = ht_task_scheduler_create(&err);

    // Assert
    ASSERT_EQ(nullptr, scheduler);
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, err);
}

TEST_F(TestTaskScheduler, CreateShouldFailIfThereIsOnlyEnoughMemoryForTaskObject)
{
    // Arrange
    LimitedSizeAllocator alloc_data(64);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);
    HT_ErrorCode err;

    // Act
    HT_TaskScheduler* scheduler = ht_task_scheduler_create(&err);

    // Assert
    ASSERT_EQ(nullptr, scheduler);
    ASSERT_EQ(HT_ERR_OUT_OF_MEMORY, err);
}

TEST_F(TestTaskScheduler, ScheduleTaskShouldFailIfThereIsNotEnoughMemory)
{
    // Arrange
    LimitedSizeAllocator alloc_data(16);
    ScopedSetAlloc allocator(&LimitedSizeAllocator::realloc, &alloc_data);

    // Act
    HT_TaskId task_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);

    // Assert
    ASSERT_EQ(HT_TASK_SCHEDULER_INVALID_TASK_ID, task_id);
}

TEST_F(TestTaskScheduler, RegisterNotNullCallbackShouldPass)
{
    // Arrange
    // Act
    HT_TaskId task_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Assert
    ASSERT_NE(HT_TASK_SCHEDULER_INVALID_TASK_ID, task_id);
}

TEST_F(TestTaskScheduler, RegisterTheSameCallbacksShouldPassAndGenerateDifferentId)
{
    // Arrange
    HT_TaskId task1_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Act
    HT_TaskId task2_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Assert
    ASSERT_NE(HT_TASK_SCHEDULER_INVALID_TASK_ID, task2_id);
    ASSERT_NE(task1_id, task2_id);
}

TEST_F(TestTaskScheduler, RemoveNonExistingTaskShouldFail)
{
    // Arrange
    HT_TaskId non_existing_id = 232;

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(_scheduler, non_existing_id);

    // Assert
    ASSERT_EQ(HT_FALSE, result);
}

TEST_F(TestTaskScheduler, RemoveNegativeTaskIdShouldFail)
{
    // Arrange
    HT_TaskId negative_id = -1;

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(_scheduler, negative_id);

    // Assert
    ASSERT_EQ(HT_FALSE, result);
}

TEST_F(TestTaskScheduler, RemoveExistingTaskShouldPass)
{
    // Arrange
    HT_TaskId task_id = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(_scheduler, task_id);

    // Assert
    ASSERT_EQ(HT_TRUE, result);
}

TEST_F(TestTaskScheduler, CallbackRegistredWithZeroPeriodShouldBeAlwaysExecuted)
{
    // Arrange
    int value = 0;
    const int tick_count = 5;
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, &value);

    // Act
    for (int i = 0; i < tick_count; i++)
    {
        ht_task_scheduler_tick(_scheduler);
    }

    // Assert
    ASSERT_EQ(tick_count, value);
}

TEST_F(TestTaskScheduler, CallbackShouldNotBeExecutedBeforeTimeout)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 5 * 1000 * 1000; // 5ms
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    ht_task_scheduler_tick(_scheduler);

    // Assert
    ASSERT_EQ(0, value);
}

TEST_F(TestTaskScheduler, CallbackShouldBeExecutedAfterTimeout)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));
    ht_task_scheduler_tick(_scheduler);

    // Assert
    ASSERT_EQ(1, value);
}

TEST_F(TestTaskScheduler, SchedulerShouldExecuteAllTasks)
{
    // Arrange
    int value1 = 0;
    int value2 = 0;
    const HT_DurationNs timeout = 20 * 1000 * 1000; // 20ms
    const HT_DurationNs period1 = 1 * 1000 * 1000; // 1ms
    const HT_DurationNs period2 = 2 * 1000 * 1000; // 2ms
    auto timestamp_start = ht_monotonic_clock_get_timestamp();
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period1, test_callback, &value1);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period2, test_callback, &value2);

    // Act
    HT_TimestampNs timestamp_now = timestamp_start;

    while ((value1 < 5 || value2 < 2) && timestamp_now < timestamp_start + timeout)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(200));

        timestamp_now = ht_monotonic_clock_get_timestamp();
        ht_task_scheduler_tick(_scheduler);
    }

    // Assert
    ASSERT_GE(timestamp_now, timestamp_start + 5000000); // check if we spent at least 5 ms executing tasks
    ASSERT_LE(timestamp_now, timestamp_start + timeout); // check timeout
}

TEST_F(TestTaskScheduler, SchedulerShouldExecuteDelayedTasks_IgnoreDelays)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));

    for (int i = 0; i < 10; i++)
    {
        ht_task_scheduler_tick(_scheduler);
    }

    // Assert
    ASSERT_LE(3, value);
}

TEST_F(TestTaskScheduler, SchedulerShouldRemoveTaskIfItReturnsFalse)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1; // 1ns
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback_false, &value);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    for (int i = 0; i < 2; i++)
    {
        ht_task_scheduler_tick(_scheduler);
        std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));
    }

    // Assert
    ASSERT_EQ(7, value);
}

TEST_F(TestTaskScheduler, SchedulerShouldNotExecuteDelayedTasks_RestartTimer)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_RESTART_TIMER, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));

    for (int i = 0; i < 10; i++)
    {
        ht_task_scheduler_tick(_scheduler);
    }

    // Assert
    ASSERT_EQ(1, value);
}

TEST_F(TestTaskScheduler, SchedulerTasksShouldAlwaysIncrease)
{
    // Arrange
    // Act
    HT_TaskId task_1 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_2 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_3 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_4 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    ht_task_scheduler_remove_task(_scheduler, task_2);
    HT_TaskId task_5 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_6 = ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);

    // Assert
    ASSERT_LT(task_1, task_2);
    ASSERT_LT(task_2, task_3);
    ASSERT_LT(task_3, task_4);
    ASSERT_LT(task_4, task_5);
    ASSERT_LT(task_5, task_6);
}

TEST_F(TestTaskScheduler, PerfectPeriodShouldReturnZeroIfNoTask)
{
    // Arrange
    // Act & Assert
    ASSERT_EQ(0u, ht_task_scheduler_get_optimal_tick_period(_scheduler));
}

TEST_F(TestTaskScheduler, PerfectPeriodShouldIgnoreTasksWithZeroPeriod)
{
    // Arrange
    const HT_DurationNs period = 20;
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, nullptr);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, nullptr);

    // Act & Assert
    ASSERT_EQ(period, ht_task_scheduler_get_optimal_tick_period(_scheduler));
}

TEST_F(TestTaskScheduler, PerfectPeriodShouldReturnCorrectValueForMultipleTasks)
{
    // Arrange
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 20, test_callback, nullptr);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, nullptr);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 20, test_callback, nullptr);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 40, test_callback, nullptr);
    ht_task_scheduler_schedule_task(_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 15, test_callback, nullptr);

    // Act & Assert
    ASSERT_EQ(5u, ht_task_scheduler_get_optimal_tick_period(_scheduler));
}
