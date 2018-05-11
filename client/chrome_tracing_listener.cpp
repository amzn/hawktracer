#include "chrome_tracing_listener.hpp"

#include "hawktracer/parser/klass_register.hpp"

namespace HawkTracer
{
namespace client
{

ChromeTraceListener::ChromeTraceListener(std::unique_ptr<TracepointMap> tracepoint_map) :
    _mapping_klass_name("HT_StringMappingEvent"),
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

    if (_mapping_klass_id == 0 &&
            event.get_klass()->get_id() == HawkTracer::parser::to_underlying(HawkTracer::parser::WellKnownKlasses::EventKlassInfoEventKlass))
    {
        if (event.get_value<char*>("event_klass_name") == _mapping_klass_name)
        {
            _mapping_klass_id = event.get_value<HT_EventKlassId>("info_klass_id");
        }
    }
    if (event.get_klass()->get_id() == _mapping_klass_id)
    {
        _tracepoint_map->add_map_entry(event.get_value<uint64_t>("identifier"), event.get_value<char*>("label"));
        return;
    }

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

    // Chrome expects the timestamps/durations to be microseconds
    // so we need to convert from nano to micro
    file << ",{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << (event.get_value<uint64_t>("timestamp") / 1000u)
         << ", \"dur\": " << (event.get_value_or_default<uint64_t>("duration", 0u) / 1000u)
         << ", \"pid\": 0, \"tid\": " << event.get_value_or_default<uint32_t>("thread_id", 0)
         << "}";
}

} // namespace client
} // namespace HawkTracer
