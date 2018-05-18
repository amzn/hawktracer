#ifndef HAWKTRACER_CLIENT_ICONVERTER_CPP
#define HAWKTRACER_CLIENT_ICONVERTER_CPP

#include "iconverter.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

IConverter::IConverter (std::shared_ptr<TracepointMap> tracepoint_map) :
    _tracepoint_map(tracepoint_map)
{
}

bool IConverter::set_tracepoint_map(std::string& map_files)
{
    if (_tracepoint_map != nullptr)
    {
        _tracepoint_map->load_maps(map_files);
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_ICONVERTER_CPP
