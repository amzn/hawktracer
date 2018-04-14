#include "hawktracer/parser/event_klass.hpp"
#include "hawktracer/parser/event.hpp"
#include "hawktracer/parser/klass_register.hpp"

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace HawkTracer {
namespace parser {

EventKlassField::EventKlassField(std::string name, std::string type_name, FieldTypeId type_id, std::shared_ptr<const EventKlass> klass) :
    _name(name), _type_name(type_name), _klass(std::move(klass)), _type_id(type_id)
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
    std::lock_guard<std::mutex> l(_fields_mtx);
    if (!_get_field(field->get_name().c_str(), false))
    {
        _fields.push_back(std::move(field));
    }
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

std::vector<std::shared_ptr<EventKlassField>> EventKlass::get_fields() const
{
    std::lock_guard<std::mutex> l(_fields_mtx);
    return _fields;
}

std::shared_ptr<const EventKlassField> EventKlass::get_field(const char* name, bool recursive) const
{
    std::lock_guard<std::mutex> l(_fields_mtx);

    return _get_field(name, recursive);
}

std::shared_ptr<const EventKlassField> EventKlass::_get_field(const char* name, bool recursive) const
{
    for (const auto& field : _fields)
    {
        if (strcmp(name, field->get_name().c_str()) == 0)
        {
            return field;
        }
        else if (field->get_type_id() == FieldTypeId::STRUCT && recursive)
        {
            auto base_field = field->get_klass()->get_field(name, recursive);
            if (base_field)
            {
                return base_field;
            }
        }
    }

    return nullptr;
}

} // namespace parser
} // namespace HawkTracer
