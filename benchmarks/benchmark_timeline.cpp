#include <hawktracer/timeline.h>

#include <benchmark/benchmark.h>

static void BenchmarkTimelineInitEvent(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(HT_BASE_TIMELINE_IDENTIFIER,
                                               HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE, HT_FALSE, nullptr);

    for (auto _ : state)
    {
        HT_DECL_EVENT(HT_Event, event);
        ht_timeline_init_event(timeline, &event);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(BenchmarkTimelineInitEvent);

static void BenchmarkTimelinePushBaseEventNoListener(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(HT_BASE_TIMELINE_IDENTIFIER,
                                               HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE, HT_FALSE, nullptr);
    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_init_event(timeline, &event);

    for (auto _ : state)
    {
        ht_timeline_push_event(timeline, &event);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(BenchmarkTimelinePushBaseEventNoListener);

static void BenchmarkTimelineFlushNoListener(benchmark::State& state)
{
    HT_Timeline* timeline = ht_timeline_create(HT_BASE_TIMELINE_IDENTIFIER,
                                               HT_BASE_TIMELINE_PROPERTY_THREAD_SAFE, HT_FALSE, nullptr);
    HT_DECL_EVENT(HT_Event, event);
    ht_timeline_init_event(timeline, &event);

    for (size_t i = 0; i < 1024; i++)
    {
        ht_timeline_push_event(timeline, &event);
    }

    for (auto _ : state)
    {
        ht_timeline_flush(timeline);
    }

    ht_timeline_destroy(timeline);
}
BENCHMARK(BenchmarkTimelineFlushNoListener);
