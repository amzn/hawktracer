#include <viewer/event_db.hpp>
#include <viewer/base_ui.hpp>
#include <hawktracer.h>
#include <benchmark/benchmark.h>
#include <hawktracer/parser/file_stream.hpp>
#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

using WellKnownKlasses = HawkTracer::parser::WellKnownKlasses;

static void fnc_bar(HT_Timeline* timeline)
{
    HT_TP_SCOPED_STRING(timeline, "bar");
}

static void fnc_foo(HT_Timeline* timeline)
{
    HT_TP_SCOPED_STRING(timeline, "foo");
    for (int i = 0; i < 10; i++)
    {
        fnc_bar(timeline);
    }
}

static void fnc_start(HT_Timeline* timeline)
{
    HT_TP_SCOPED_STRING(timeline, "start");
    for (int i = 0; i < 100; i++)
    {
        fnc_foo(timeline);
    }
}

static void handle_event(const HawkTracer::parser::Event& event, HawkTracer::viewer::EventDB& event_db, HawkTracer::viewer::TimeRange& total_ts_range)
{
    const auto& klass = event.get_klass();

    switch(static_cast<WellKnownKlasses>(klass->get_id()))
    {
        case WellKnownKlasses::EventKlassInfoEventKlass:
        case WellKnownKlasses::EventKlassFieldInfoEventKlass:
            break;
        default:
            event_db.insert(event);
            auto ts = event.get_timestamp();
            if (ts < total_ts_range.start)
            {
                total_ts_range.start = ts;
            }
            if (ts > total_ts_range.stop) 
            {
                total_ts_range.stop = ts;
            }
    }
}

static void BenchmarkEventDbGetData(benchmark::State& state)
{
    HawkTracer::viewer::TimeRange total_ts_range(0, 0);

    // Create timeline
    HT_Timeline timeline;
    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, nullptr);
    ht_feature_callstack_enable(&timeline);

    HT_FileDumpListener file_dump_listener;
    if (ht_file_dump_listener_init(&file_dump_listener, "test_output.htdump", 4096u) != HT_FALSE)
    {
        ht_timeline_register_listener(&timeline, ht_file_dump_listener_callback, &file_dump_listener);
    }

    // Add events
    ht_registry_push_all_klass_info_events(&timeline);
    fnc_start(&timeline);

    ht_timeline_flush(&timeline);
    ht_timeline_unregister_all_listeners(&timeline);
    ht_file_dump_listener_deinit(&file_dump_listener);

    // Arrange
    auto stream = HawkTracer::parser::make_unique<HawkTracer::parser::FileStream>("test_output.htdump");
    HawkTracer::parser::KlassRegister klass_register;
    auto reader = HawkTracer::parser::make_unique<HawkTracer::parser::ProtocolReader>(&klass_register, std::move(stream), true);
    HawkTracer::viewer::EventDB event_db;
    reader->register_events_listener([&event_db, &total_ts_range](const HawkTracer::parser::Event& event) {
            handle_event(event, event_db, total_ts_range);
            });

    // Run viewer
    reader->start();
    reader->wait_for_complete();

    // Query events
    for (auto _ : state)
    {
        HT_TimestampNs start_ts = (HT_TimestampNs)total_ts_range.start;
        HT_TimestampNs stop_ts = (HT_TimestampNs)total_ts_range.stop;
        Query query;
        auto result = event_db.get_data(start_ts, stop_ts, query);
    }
    ht_timeline_deinit(&timeline);
}
BENCHMARK(BenchmarkEventDbGetData);

