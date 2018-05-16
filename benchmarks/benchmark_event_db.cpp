#include <viewer/event_db.hpp>
#include <viewer/base_ui.hpp>
#include <hawktracer.h>
#include <benchmark/benchmark.h>
#include <hawktracer/parser/file_stream.hpp>
#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

using HawkTracer::parser::WellKnownKlasses;

static void generate_test_tracepoints(HT_Timeline* timeline, int tracepoints)
{
    for (int i = 0; i < tracepoints; i++)
    {
        HT_DECL_EVENT(HT_Event, event);
        ht_timeline_push_event(timeline, &event);
    }
}

static void handle_event(const HawkTracer::parser::Event& event, HawkTracer::viewer::EventDB& event_db, HawkTracer::viewer::TimeRange& total_ts_range)
{
    const auto& klass = event.get_klass();

    switch (static_cast<WellKnownKlasses>(klass->get_id()))
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
    // Create timeline
    HT_Timeline timeline;
    ht_timeline_init(&timeline, 1024, HT_FALSE, HT_FALSE, nullptr);

    HT_FileDumpListener file_dump_listener;
    const char* filename = "test_output.htdump";
    HT_ErrorCode error_code = ht_file_dump_listener_init(&file_dump_listener, filename, 4096u);
    if (error_code != HT_ERR_OK)
    {
        char buffer[100];
        snprintf(buffer, 100, "Failed to initialize file dump listener! Error code: %d", error_code);
        state.SkipWithError(buffer);
        return;
    }
    ht_timeline_register_listener(&timeline, ht_file_dump_listener_callback, &file_dump_listener);

    // Add events
    ht_registry_push_all_klass_info_events(&timeline);
    generate_test_tracepoints(&timeline, state.range(0));

    ht_timeline_deinit(&timeline);
    ht_file_dump_listener_deinit(&file_dump_listener);

    // Arrange
    auto stream = HawkTracer::parser::make_unique<HawkTracer::parser::FileStream>(filename);
    HawkTracer::parser::KlassRegister klass_register;
    auto reader = HawkTracer::parser::make_unique<HawkTracer::parser::ProtocolReader>(&klass_register, std::move(stream), true);
    HawkTracer::viewer::EventDB event_db(HawkTracer::parser::make_unique<HawkTracer::viewer::Cache>());
    HawkTracer::viewer::TimeRange total_ts_range(0, 0);
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

    remove(filename);
}
// Passing the number of tracepoints to generate as the first argument
BENCHMARK(BenchmarkEventDbGetData)->Arg(100);

