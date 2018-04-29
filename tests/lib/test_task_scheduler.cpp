#include <hawktracer/task_scheduler.h>

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

static void test_callback(void* ud)
{
    int* value = static_cast<int*>(ud);
    (*value)++;
}

class TestTaskScheduler : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ht_task_scheduler_init(&_scheduler);
    }

    void TearDown()
    {
        ht_task_scheduler_deinit(&_scheduler);
    }

    HT_TaskScheduler _scheduler;
};

TEST_F(TestTaskScheduler, RegisterNullCallbackShouldFail)
{
    // Arrange
    // Act
    HT_TaskId task_id = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, NULL, NULL);

    // Assert
    ASSERT_EQ(HT_TASK_SCHEDULER_INVALID_TASK_ID, task_id);
}

TEST_F(TestTaskScheduler, RegisterNotNullCallbackShouldPass)
{
    // Arrange
    // Act
    HT_TaskId task_id = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Assert
    ASSERT_NE(HT_TASK_SCHEDULER_INVALID_TASK_ID, task_id);
}

TEST_F(TestTaskScheduler, RegisterTheSameCallbacksShouldPassAndGenerateDifferentId)
{
    // Arrange
    HT_TaskId task1_id = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Act
    HT_TaskId task2_id = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Assert
    ASSERT_NE(HT_TASK_SCHEDULER_INVALID_TASK_ID, task2_id);
    ASSERT_NE(task1_id, task2_id);
}

TEST_F(TestTaskScheduler, RemoveNonExistingTaskShouldFail)
{
    // Arrange
    HT_TaskId non_existing_id = 232;

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(&_scheduler, non_existing_id);

    // Assert
    ASSERT_EQ(HT_FALSE, result);
}

TEST_F(TestTaskScheduler, RemoveNegativeTaskIdShouldFail)
{
    // Arrange
    HT_TaskId negative_id = -1;

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(&_scheduler, negative_id);

    // Assert
    ASSERT_EQ(HT_FALSE, result);
}

TEST_F(TestTaskScheduler, RemoveExistingTaskShouldPass)
{
    // Arrange
    HT_TaskId task_id = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, NULL);

    // Act
    HT_Boolean result = ht_task_scheduler_remove_task(&_scheduler, task_id);

    // Assert
    ASSERT_EQ(HT_TRUE, result);
}

TEST_F(TestTaskScheduler, CallbackRegistredWithZeroPeriodShouldBeAlwaysExecuted)
{
    // Arrange
    int value = 0;
    const int tick_count = 5;
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, &value);

    // Act
    for (int i = 0; i < tick_count; i++)
    {
        ht_task_scheduler_tick(&_scheduler);
    }

    // Assert
    ASSERT_EQ(tick_count, value);
}

TEST_F(TestTaskScheduler, CallbackShouldNotBeExecutedBeforeTimeout)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 5 * 1000 * 1000; // 5ms
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    ht_task_scheduler_tick(&_scheduler);

    // Assert
    ASSERT_EQ(0, value);
}

TEST_F(TestTaskScheduler, CallbackShouldBeExecutedAfterTimeout)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));
    ht_task_scheduler_tick(&_scheduler);

    // Assert
    ASSERT_EQ(1, value);
}

TEST_F(TestTaskScheduler, SchedulerShouldExecuteAllTasks)
{
    // Arrange
    int value1 = 0;
    int value2 = 0;
    const HT_DurationNs period1 = 1 * 1000 * 1000; // 1ms
    const HT_DurationNs period2 = 2 * 1000 * 1000; // 2ms
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period1, test_callback, &value1);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period2, test_callback, &value2);

    // Act
    for (int i = 0; i < 5; i++)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1100));
        ht_task_scheduler_tick(&_scheduler);
    }

    // Assert
    ASSERT_EQ(5, value1);
    ASSERT_EQ(2, value2);
}

TEST_F(TestTaskScheduler, SchedulerShouldExecuteDelayedTasks_IgnoreDelays)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));

    for (int i = 0; i < 10; i++)
    {
        ht_task_scheduler_tick(&_scheduler);
    }

    // Assert
    ASSERT_EQ(3, value);
}

TEST_F(TestTaskScheduler, SchedulerShouldNotExecuteDelayedTasks_RestartTimer)
{
    // Arrange
    int value = 0;
    const HT_DurationNs period = 1 * 1000 * 1000; // 1ms
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_RESTART_TIMER, period, test_callback, &value);

    // Act
    std::this_thread::sleep_for(std::chrono::nanoseconds(period * 3));

    for (int i = 0; i < 10; i++)
    {
        ht_task_scheduler_tick(&_scheduler);
    }

    // Assert
    ASSERT_EQ(1, value);
}

TEST_F(TestTaskScheduler, SchedulerTasksShouldAlwaysIncrease)
{
    // Arrange
    // Act
    HT_TaskId task_1 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_2 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_3 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_4 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    ht_task_scheduler_remove_task(&_scheduler, task_2);
    HT_TaskId task_5 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);
    HT_TaskId task_6 = ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 10, test_callback, nullptr);

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
    ASSERT_EQ(0u, ht_task_scheduler_get_optimal_tick_period(&_scheduler));
}

TEST_F(TestTaskScheduler, PerfectPeriodShouldIgnoreTasksWithZeroPeriod)
{
    // Arrange
    const HT_DurationNs period = 20;
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, period, test_callback, nullptr);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, nullptr);

    // Act & Assert
    ASSERT_EQ(period, ht_task_scheduler_get_optimal_tick_period(&_scheduler));
}

TEST_F(TestTaskScheduler, PerfectPeriodShouldReturnCorrectValueForMultipleTasks)
{
    // Arrange
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 20, test_callback, nullptr);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 0, test_callback, nullptr);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 20, test_callback, nullptr);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 40, test_callback, nullptr);
    ht_task_scheduler_schedule_task(&_scheduler, HT_TASK_SCHEDULING_IGNORE_DELAYS, 15, test_callback, nullptr);

    // Act & Assert
    ASSERT_EQ(5u, ht_task_scheduler_get_optimal_tick_period(&_scheduler));
}
