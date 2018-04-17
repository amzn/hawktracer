#include <hawktracer/timeline.h>

#include <benchmark/benchmark.h>

static void BenchmarkTimelineInitEvent(benchmark::State& state)
{
    HT_Timeline timeline;
    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, NULL);

    for (auto _ : state)
    {
        HT_DECL_EVENT(HT_Event, event);
        ht_timeline_init_event(&timeline, &event);
    }

    ht_timeline_deinit(&timeline);
}
BENCHMARK(BenchmarkTimelineInitEvent);

static void BenchmarkTimelinePushBaseEventNoListener(benchmark::State& state)
{
    HT_Timeline timeline;
    ht_timeline_init(&timeline, state.range(0), HT_FALSE, HT_FALSE, NULL);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_init_event(&timeline, &event);
    for (auto _ : state)
    {
        ht_timeline_push_event(&timeline, &event);
    }

    ht_timeline_deinit(&timeline);
}
// Passing the capacity of the timeline's buffer as the first argument
BENCHMARK(BenchmarkTimelinePushBaseEventNoListener)->Arg(1024)->Arg(10);

static void BenchmarkTimelineFlushNoListener(benchmark::State& state)
{
    HT_Timeline timeline;
    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, NULL);

    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_init_event(&timeline, &event);

    for (size_t i = 0; i < 1024; i++)
    {
        ht_timeline_push_event(&timeline, &event);
    }

    for (auto _ : state)
    {
        ht_timeline_flush(&timeline);
    }

    ht_timeline_deinit(&timeline);
}
BENCHMARK(BenchmarkTimelineFlushNoListener);
