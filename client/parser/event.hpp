#ifndef HAWKTRACER_PARSER_EVENT_HPP
#define HAWKTRACER_PARSER_EVENT_HPP

#include "parser/event_klass.hpp"

#include <unordered_map>

namespace HawkTracer
{
namespace parser
{

class Event
{
public:
    struct Value
    {
        Value(FieldType value, const EventKlassField* field) :
            value(std::move(value)), field(field)
        {
        }
        Value(Value&&) = default;

        FieldType value;
        const EventKlassField* field;
    };

    explicit Event(const EventKlass& klass) :
        _klass(klass)
    {
    }
    ~Event();
    Event(Event&&) = default;

    void merge(Event event);

    template<typename T>
    T get_value(const std::string& key) const;

    template<typename T>
    T get_value_or_default(const std::string& key, const T& default_value) const
    {
        auto it = _values.find(key);
        return (it != _values.end()) ? get_value<T>(key) : default_value;
    }

    bool has_value(const std::string& key) const
    {
        return _values.find(key) != _values.end();
    }

    const Value& get_raw_value(const std::string& key) const
    {
        return _values.at(key);
    }

    void set_value(const EventKlassField* field, FieldType value)
    {
        _values.insert(std::make_pair(field->get_name(), std::move(Value(std::move(value), field))));
    }

    const EventKlass& get_klass() const { return _klass; }

    const std::unordered_map<std::string, Value>& get_values() const { return _values; }

private:
    std::unordered_map<std::string, Value> _values;
    const EventKlass& _klass;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_EVENT_HPP
