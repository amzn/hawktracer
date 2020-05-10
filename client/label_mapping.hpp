#ifndef HAWKTRACER_CLIENT_LABEL_MAPPING_HPP
#define HAWKTRACER_CLIENT_LABEL_MAPPING_HPP

#include "tracepoint_map.hpp"

#include <hawktracer/parser/event.hpp>
#include <hawktracer/parser/klass_register.hpp>

#include <memory>
#include <string>

namespace HawkTracer
{
namespace client
{

class LabelMapping
{
public:
    LabelMapping(std::unique_ptr<TracepointMap> tracepoint_map):
        _tracepoint_map(std::move(tracepoint_map))
    {
    }

    std::string process_event(const parser::Event& event)
    {
        std::string label;

        if (_mapping_klass_id == 0)
        {
            _try_setting_mapping_klass_id(event);
        }
        else if (event.get_klass()->get_id() == _mapping_klass_id)
        {
            _tracepoint_map->add_map_entry(event.get_value<uint64_t>("identifier"), event.get_value<char*>("label"));
        }
        else if (event.has_value("label"))
        {
            label = _convert_value_to_string(event.get_raw_value("label"));
        }
        else if (event.has_value("name"))
        {
            label = event.get_value<char*>("name");
        }
        return label;
    }

private:
    void _try_setting_mapping_klass_id(const parser::Event& event)
    {
        if (event.get_klass()->get_id() == HawkTracer::parser::to_underlying(HawkTracer::parser::WellKnownKlasses::EventKlassInfoEventKlass))
        {
            if (event.get_value<char*>("event_klass_name") == _mapping_klass_name)
            {
                _mapping_klass_id = event.get_value<HT_EventKlassId>("info_klass_id");
            }
        }
    }

    std::string _convert_value_to_string(const parser::Event::Value& value)
    {
        switch (value.field->get_type_id())
        {
        case parser::FieldTypeId::UINT64:
            return _tracepoint_map->get_label_info(value.value.f_UINT64).label;
        case parser::FieldTypeId::STRING:
            return value.value.f_STRING;
        default:
            return "invalid label type";
        }
    }

    std::unique_ptr<TracepointMap> _tracepoint_map;
    std::string _mapping_klass_name = "HT_StringMappingEvent";
    HT_EventKlassId _mapping_klass_id = 0;
};


} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_LABEL_MAPPING_HPP
