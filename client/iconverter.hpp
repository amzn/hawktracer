#ifndef HAWKTRACER_CLIENT_ICONVERTER_HPP
#define HAWKTRACER_CLIENT_ICONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "tracepoint_map.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

class IConverter
{
public:
    virtual ~IConverter() {}
    virtual bool init(const std::string& file_name) = 0;
    virtual void uninit() = 0;
    virtual void process_event(const parser::Event& event) = 0;
    bool set_tracepoint_map(std::string& map_files);

protected:
    IConverter(std::shared_ptr<TracepointMap> tracepoint_map);
    HT_EventKlassId _mapping_klass_id = 0;
    std::shared_ptr<TracepointMap> _tracepoint_map;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_ICONVERTER_HPP
