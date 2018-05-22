#ifndef HAWKTRACER_CLIENT_CONVERTER_CPP
#define HAWKTRACER_CLIENT_CONVERTER_CPP

#include "converter.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

Converter::Converter()
{
    _tracepoint_map = std::make_shared<TracepointMap>();
}

bool Converter::set_tracepoint_map(const std::string& map_files)
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

#endif // HAWKTRACER_CLIENT_CONVERTER_CPP
