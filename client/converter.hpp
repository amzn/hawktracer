#ifndef HAWKTRACER_CLIENT_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "tracepoint_map.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

class Converter
{
public:
    Converter();

    virtual ~Converter() {}
    virtual bool init(const std::string& file_name) = 0;
    virtual void process_event(const parser::Event& event) = 0;
    bool set_tracepoint_map(const std::vector<std::string>& map_files);
    virtual void stop() = 0;

protected:
    std::string _get_label(const parser::Event& event);
    std::unique_ptr<TracepointMap> _tracepoint_map;

private:
    void _try_setting_mapping_klass_id(const parser::Event& event);
    std::string _convert_value_to_string(const parser::Event::Value& value);
    const std::string _mapping_klass_name;
    HT_EventKlassId _mapping_klass_id = 0;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_HPP
