#include "debug_event_listener.hpp"

#include <iostream>
#include <cassert>

namespace HawkTracer
{
namespace parser
{

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

static void print_field_value(const Event::Value& value)
{
#define PRINT_FIELD_VALUE(TYPE) case FieldTypeId::TYPE: std::cout << value.value.f_##TYPE << std::endl; break

    switch (value.field->get_type_id())
    {
    PRINT_FIELD_VALUE(UINT8);
    PRINT_FIELD_VALUE(INT8);
    PRINT_FIELD_VALUE(UINT16);
    PRINT_FIELD_VALUE(INT16);
    PRINT_FIELD_VALUE(UINT32);
    PRINT_FIELD_VALUE(INT32);
    PRINT_FIELD_VALUE(UINT64);
    PRINT_FIELD_VALUE(INT64);
    PRINT_FIELD_VALUE(STRING);
    PRINT_FIELD_VALUE(POINTER);
    default: assert(0);
    }
}

static void print_event(const Event& event, size_t indent)
{
    for (const auto& value : event.get_values())
    {
        std::cout << std::string(indent, ' ');
        std::cout << value.second.field->get_type_name() << ' ' << value.second.field->get_name() << ": ";
        
        if (value.second.field->get_type_id() == FieldTypeId::STRUCT)
        {
            std::cout << std::endl;
            print_event(*value.second.value.f_EVENT, indent + 2);
        }
        else
        {
            print_field_value(value.second);
        }
    }
}

void DebugEventListener::_process_event(const Event& event)
{
    std::cout << std::endl << event.get_klass().get_name() << std::endl;
    print_event(event, 2);

    _event_stats[event.get_klass().get_name()]++;
}

void DebugEventListener::print_statistics() const
{
    std::cout << "Statistics: " << std::endl;
    size_t total_count = 0;
    for (const auto& klass : _event_stats)
    {
        std::cout << "  " << klass.first << ": " << klass.second << std::endl;
        total_count += klass.second;
    }
    std::cout << "Total number of events: " << total_count << std::endl;
}

} // namespace parser
} // namespace HawkTracer
