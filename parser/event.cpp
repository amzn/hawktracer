#include "event.hpp"

namespace HawkTracer
{
namespace parser
{

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

#define EVENT_GET_VALUE(C_TYPE, UNION_FIELD) \
    template<> C_TYPE Event::get_value(const std::string& key) const \
    { \
        return _values.at(key).value.UNION_FIELD; \
    }

EVENT_GET_VALUE(uint8_t, f_UINT8)
EVENT_GET_VALUE(int8_t, f_UINT8)
EVENT_GET_VALUE(uint16_t, f_UINT16)
EVENT_GET_VALUE(int16_t, f_INT16)
EVENT_GET_VALUE(uint32_t, f_UINT32)
EVENT_GET_VALUE(int32_t, f_INT32)
EVENT_GET_VALUE(uint64_t, f_UINT64)
EVENT_GET_VALUE(int64_t, f_INT64)
EVENT_GET_VALUE(void*, f_POINTER)
EVENT_GET_VALUE(char*, f_STRING)
EVENT_GET_VALUE(Event*, f_EVENT)


} // namespace parser
} // namespace HawkTracer
