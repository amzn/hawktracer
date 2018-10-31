#include "hawktracer/parser/event.hpp"

#include <cstring>
#include <cstdlib>

namespace HawkTracer
{
namespace parser
{

Event::Value::Value(Value&& other) :
    Value()
{
    _swap(other);
}

Event::Value& Event::Value::operator=(Value&& other)
{
    _swap(other);
    other.field = nullptr;
    return *this;
}

Event::Value::Value(const Value& other)
{
    field = other.field;

    if (!field)
    {
        return;
    }

    switch (field->get_type_id())
    {
    case FieldTypeId::STRING:
        if (other.value.f_STRING)
        {
            value.f_STRING = (char*)malloc(strlen(other.value.f_STRING) + 1);
            strcpy(value.f_STRING, other.value.f_STRING);
        }
        else
        {
            value.f_STRING = nullptr;
        }
        break;
    case FieldTypeId::STRUCT:
        if (other.value.f_EVENT)
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

Event::Value& Event::Value::operator=(Value other)
{
    _swap(other);
    return *this;
}

void Event::Value::_swap(Value& other)
{
    std::swap(field, other.field);
    std::swap(value, other.value);
}

Event::~Event()
{
    for (const auto& value : _values)
    {
        if (value.second.field == nullptr)
        {
            continue;
        }

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
    _values.insert(std::make_pair(field->get_name(), Value(std::move(value), field)));

    if (field->get_name() == "timestamp")
    {
        _timestamp = get_value<HT_TimestampNs>("timestamp");
    }
}

template<typename T>
void Event::set_value(const EventKlassField* field, T value)
{
    FieldType field_value = {};
    *(T*)&field_value = value;
    set_value(field, field_value);
}

template<typename T>
T Event::get_value(const std::string& key) const
{
    return *(T*)&_values.at(key).value;
}

#define EVENT_GET_SET_VALUE(C_TYPE, _UNUSED) \
    template C_TYPE Event::get_value<C_TYPE>(const std::string& key) const; \
    template void Event::set_value<C_TYPE>(const EventKlassField* field, C_TYPE value);

#define C_TYPE_LIST uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, void*, char*, Event*
MKCREFLECT_FOREACH(EVENT_GET_SET_VALUE, 0, C_TYPE_LIST)

} // namespace parser
} // namespace HawkTracer
