#include "chrome_tracing_listener.hpp"

namespace HawkTracer
{
namespace client
{

ChromeTraceListener::ChromeTraceListener(std::unique_ptr<TracepointMap> tracepoint_map) :
    _tracepoint_map(std::move(tracepoint_map))
{
}

ChromeTraceListener::~ChromeTraceListener()
{
    uninit();
}

bool ChromeTraceListener::init(const std::string& file_name)
{
    file.open(file_name);
    if (file.is_open())
    {
        file << "{\"traceEvents\": [ {} ";
        return true;
    }
    return false;
}

void ChromeTraceListener::uninit()
{
    if (file.is_open())
    {
        file << "]}";
        file.close();
    }
}

void ChromeTraceListener::process_event(const parser::Event& event)
{
    std::string label;
    if (event.has_value("label"))
    {
        const parser::Event::Value& value = event.get_raw_value("label");
        if (value.field->get_type_id() == parser::FieldTypeId::UINT64)
        {
            label = _tracepoint_map->get_label_info(value.value.f_UINT64).label;
        }
        else if (value.field->get_type_id() == parser::FieldTypeId::STRING)
        {
            label = value.value.f_STRING;
        }
        else
        {
            label = "invalid label type";
        }
    }
    else if (event.has_value("name"))
    {
        label = event.get_value<char*>("name");
    }
    else
    {
        return;
    }
    file << ",{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << event.get_value<uint64_t>("timestamp")
         << ", \"dur\": " << event.get_value_or_default<uint64_t>("duration", 0u)
         << ", \"pid\": 0, \"tid\": " << event.get_value_or_default<uint32_t>("thread_id", 0)
         << "}";
}

} // namespace client
} // namespace HawkTracer
