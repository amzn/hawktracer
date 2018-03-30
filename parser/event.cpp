#include "event.hpp"

#include <cstring>

namespace HawkTracer
{
namespace parser
{

Event::Value::Value(const Value& other) :
    field(other.field)
{
    switch (field->get_type_id())
    {
    case FieldTypeId::STRING:
        if (other.value.f_STRING)
        {
            value.f_STRING = (char*)malloc(strlen(other.value.f_STRING));
            strcpy(value.f_STRING, other.value.f_STRING);
        }
        else
        {
            value.f_STRING = nullptr;
        }
        break;
    case FieldTypeId::STRUCT:
        if (value.f_EVENT)
        {
            value.f_EVENT = new Event(*other.value.f_EVENT);
        }
        else
        {
            value.f_EVENT = nullptr;
        }
        break;
    default:
        value = other.value;
    }
}

Event::~Event()
{
    for (const auto& value : _values)
    {
        switch (value.second.field->get_type_id())
        {
        case FieldTypeId::STRING:
            free(value.second.value.f_STRING);
            break;
        case FieldTypeId::STRUCT:
            delete value.second.value.f_EVENT;
            break;
        default:
            break;
        }
    }
}

void Event::merge(Event event)
{
    for (auto& value : event._values)
    {
        set_value(value.second.field, value.second.value);
        switch (value.second.field->get_type_id())
        {
        case FieldTypeId::STRING:
            value.second.value.f_STRING = nullptr;
            break;
        case FieldTypeId::STRUCT:
            value.second.value.f_EVENT = nullptr;
            break;
        default:
            break;
        }
    }
}

void Event::set_value(const EventKlassField* field, FieldType value)
{
    _values.insert(std::make_pair(field->get_name(), std::move(Value(std::move(value), field))));

    if (field->get_name() == "timestamp")
    {
        _timestamp = get_value<HT_TimestampNs>("timestamp");
    }
}

template<typename T>
T Event::get_value(const std::string& key) const
{
    return *(T*)&_values.at(key).value;
}

#define EVENT_GET_VALUE(C_TYPE, _UNUSED) \
    template C_TYPE Event::get_value<C_TYPE>(const std::string& key) const;

MKCREFLECT_FOREACH(EVENT_GET_VALUE, 0, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t)
MKCREFLECT_FOREACH(EVENT_GET_VALUE, 0, uint64_t, int64_t, void*, char*, Event*)

} // namespace parser
} // namespace HawkTracer
