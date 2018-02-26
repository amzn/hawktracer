#include "tracepoint_map.hpp"

#include <fstream>
#include <iostream>

namespace HawkTracer
{
namespace client
{

bool TracepointMap::load_map(const std::string& map_file)
{
    std::ifstream file;
    file.open(map_file);

    if (!file.is_open())
    {
        return false;
    }

    std::string label_str;
    uint64_t label;
    uint32_t category;
    while (file >> label >> category >> label_str)
    {
        _input_map[label] = MapInfo{ label_str, category_to_string((Category)category) };
    }

    return true;
}

void TracepointMap::load_maps(const std::string& map_files)
{
    size_t start = 0;
    size_t len;
    do
    {
        size_t pos = map_files.find(',', start);
        len = pos == std::string::npos ? pos : pos - start;
        std::string path = map_files.substr(start, len);
        start += len + 1;

        if (!load_map(path))
        {
            std::cerr << "unable to load map file " << path << std::endl;
        }
    } while (len != std::string::npos);
}

TracepointMap::MapInfo TracepointMap::get_label_info(uint64_t label)
{
    auto it = _input_map.find(label);

    if (it != _input_map.end())
    {
        return it->second;
    }

    MapInfo info = { std::to_string(label), category_to_string(Unknown) };
    _input_map[label] = info;

    std::cerr << "Cannot find mapping for label " << label << std::endl;
    return info;
}

std::string TracepointMap::category_to_string(Category category)
{
    switch (category)
    {
    case Lua:
        return "lua";
    case Native:
        return "native";
    default:
        return "default";
    }
}

} // client
} // HawkTracer
