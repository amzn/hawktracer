#include "event_klass.hpp"
#include "event.hpp"

#include <cassert>
#include <stdexcept>

namespace HawkTracer {
namespace parser {

EventKlassField::EventKlassField(std::string name, std::string type_name, FieldTypeId type_id) :
    _name(name), _type_name(type_name), _type_id(type_id)
{
}

size_t EventKlassField::get_sizeof() const
{
    assert(is_numeric());
    FieldType v;
#define GET_SIZE_VARIANT(type_id) case FieldTypeId::type_id: return sizeof(v.f_##type_id)
    switch (_type_id)
    {
    GET_SIZE_VARIANT(INT8);
    GET_SIZE_VARIANT(UINT8);
    GET_SIZE_VARIANT(INT16);
    GET_SIZE_VARIANT(UINT16);
    GET_SIZE_VARIANT(INT32);
    GET_SIZE_VARIANT(UINT32);
    GET_SIZE_VARIANT(INT64);
    GET_SIZE_VARIANT(UINT64);
    default: assert(0); return 0;
    }
#undef GET_SIZE_VARIANT
}

EventKlass::EventKlass(const std::string& name, uint32_t id) :
    _name(name), _id(id)
{
}

void EventKlass::add_field(std::unique_ptr<EventKlassField> field)
{
    _fields.push_back(std::move(field));
}


FieldTypeId get_type_id(uint64_t type_size, MKCREFLECT_Types data_type)
{
    switch (data_type)
    {
    case MKCREFLECT_TYPES_POINTER:
        return FieldTypeId::POINTER;
    case MKCREFLECT_TYPES_STRING:
        return FieldTypeId::STRING;
    case MKCREFLECT_TYPES_INTEGER:
        switch (type_size)
        {
        case 1: return FieldTypeId::UINT8;
        case 2: return FieldTypeId::UINT16;
        case 4: return FieldTypeId::UINT32;
        case 8: return FieldTypeId::UINT64;
        }
    case MKCREFLECT_TYPES_STRUCT:
        return FieldTypeId::STRUCT;
    default: assert(0); // TODO other types
        throw std::runtime_error("invalid type id");
    }
}

} // namespace parser
} // namespace HawkTracer
