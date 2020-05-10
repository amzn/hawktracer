#include "chrome_trace_converter.hpp"

#include <set>

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
    stop();
}

bool ChromeTraceConverter::init(const std::string& file_name)
{
    _first_event_saved = false;
    _file.open(file_name);
    if (_file.is_open())
    {
        _file << "{\"traceEvents\": [";
        return true;
    }
    return false;
}

void ChromeTraceConverter::process_event(const parser::Event& event, const std::string& reader_id, int64_t offset_ns)
{
    std::lock_guard<std::mutex> l(_file_mtx);
    std::string label = _label_mapping->process_event(event);

    if (label == "")
    {
        return;
    }

    if (_first_event_saved)
    {
        _file << ",";
    }
    else
    {
        _first_event_saved = true;
    }

    // Chrome expects the timestamps/durations to be microseconds
    // so we need to convert from nano to micro
    _file << "{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << ns_to_ms(event.get_timestamp() + offset_ns)
         << ", \"dur\": " << ns_to_ms(event.get_value_or_default<HT_DurationNs>("duration", 0u))
         << ", \"pid\": \"" << reader_id << "\", \"tid\": " << event.get_value_or_default<HT_ThreadId>("thread_id", 0u)
         << ", \"args\": {" << _get_args(event) << "}"
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

std::string ChromeTraceConverter::_get_args(const parser::Event& event)
{
    static std::set<std::string> core_fields = {"thread_id", "timestamp", "klass_id", "id", "label", "duration"};

    std::string ret;
    bool is_first = true;

    for (const auto& value : event.get_values())
    {
        if (core_fields.find(value.first) != core_fields.end())
        {
            continue;
        }
        if (is_first)
        {
            is_first = false;
        }
        else
        {
            ret += ",";
        }
        ret += "\"" + value.first + "\": " + _get_json_value(value.second);
    }
    return ret;
}

std::string ChromeTraceConverter::_get_json_value(const parser::Event::Value& value)
{
#define PRINT_INT_VALUE(TYPE, _UNUSED) \
    case parser::FieldTypeId::TYPE: return std::to_string(value.value.f_##TYPE);
#define INT_TYPE_LIST UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64


    switch (value.field->get_type_id())
    {
    MKCREFLECT_FOREACH(PRINT_INT_VALUE, 0, INT_TYPE_LIST)
    case parser::FieldTypeId::STRING:
        return std::string("\"") + value.value.f_STRING + "\"";
    default:
        return "\"unsupported type\"";
    }
}

} // namespace client
} // namespace HawkTracer
