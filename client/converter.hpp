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
    virtual ~Converter() {}
    virtual bool init(const std::string& file_name) = 0;
    virtual void uninit() = 0;
    virtual void process_event(const parser::Event& event) = 0;
    bool set_tracepoint_map(const std::string& map_files);
    virtual bool stop() = 0;

protected:
    Converter();
    std::string get_label(const parser::Event& event);
    const std::string _mapping_klass_name; 
    HT_EventKlassId _mapping_klass_id = 0;
    std::shared_ptr<TracepointMap> _tracepoint_map;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_HPP
