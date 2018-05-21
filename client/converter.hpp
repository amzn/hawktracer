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
    virtual void uninit() = 0;
    virtual void process_event(const parser::Event& event) = 0;
    bool set_tracepoint_map(const std::string& map_files);

protected:
    HT_EventKlassId _mapping_klass_id = 0;
    std::unique_ptr<TracepointMap> _tracepoint_map;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_HPP
