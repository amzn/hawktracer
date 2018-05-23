#include "chrome_trace_converter.hpp"

static uint64_t ns_to_ms(uint64_t nano_secs)
{
    return nano_secs / 1000u;
}

namespace HawkTracer
{
namespace client
{

ChromeTraceConverter::~ChromeTraceConverter()
{
}

bool ChromeTraceConverter::init(const std::string& file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        _file << "{\"traceEvents\": [ {} ";
        return true;
    }
    return false;
}

void ChromeTraceConverter::process_event(const parser::Event& event)
{
    std::string label = _get_label(event);
    
    if (label == "")
    {
        return;
    }

    // Chrome expects the timestamps/durations to be microseconds
    // so we need to convert from nano to micro
    _file << ",{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << ns_to_ms(event.get_value<HT_TimestampNs>("timestamp"))
         << ", \"dur\": " << ns_to_ms(event.get_value_or_default<HT_DurationNs>("duration", 0u))
         << ", \"pid\": 0, \"tid\": " << event.get_value_or_default<HT_ThreadId>("thread_id", 0)
         << "}";
}

void ChromeTraceConverter::stop()
{
    if (_file.is_open())
    {
        _file << "]}";
        _file.close();
    }
}

} // namespace client
} // namespace HawkTracer
