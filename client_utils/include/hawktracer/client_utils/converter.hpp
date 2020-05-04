#ifndef HAWKTRACER_CLIENT_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CONVERTER_HPP

#include <hawktracer/client_utils/tracepoint_map.hpp>
#include <hawktracer/parser/event.hpp>

namespace HawkTracer
{
namespace ClientUtils
{

class Converter
{
public:
    Converter();

    virtual ~Converter() {}
    virtual bool init(const std::string& /*file_name*/) { return true; }
    virtual void process_event(const parser::Event& /*event*/) {}
    bool set_tracepoint_map(const std::string& map_files);
    virtual void stop() {}

    std::string get_label(const parser::Event& event) { return _get_label(event); }

protected:
    std::string _get_label(const parser::Event& event);
    std::unique_ptr<TracepointMap> _tracepoint_map;

private:
    void _try_setting_mapping_klass_id(const parser::Event& event);
    std::string _convert_value_to_string(const parser::Event::Value& value);
    const std::string _mapping_klass_name;
    HT_EventKlassId _mapping_klass_id = 0;
};

} // namespace ClientUtils
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_HPP
