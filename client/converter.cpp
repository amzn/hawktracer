#ifndef HAWKTRACER_CLIENT_CONVERTER_CPP
#define HAWKTRACER_CLIENT_CONVERTER_CPP

#include "converter.hpp"

#include <hawktracer/parser/klass_register.hpp>
#include <hawktracer/parser/make_unique.hpp>
#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

Converter::Converter() :
    _mapping_klass_name("HT_StringMappingEvent"),
    _tracepoint_map(HawkTracer::parser::make_unique<TracepointMap>())
{
}

bool Converter::set_tracepoint_map(const std::string& map_files)
{
    if (_tracepoint_map != nullptr)
    {
        _tracepoint_map->load_maps(map_files);
        return true;
    }
    else
    {
        return false;
    }
}

std::string Converter::get_label(const parser::Event& event)
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
        return label;
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
        return label;
    }
    return label;
}

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_CPP
