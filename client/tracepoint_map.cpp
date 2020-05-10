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
    std::string category;
    while (file >> category >> label_str >> label)
    {
        _input_map[label] = MapInfo{ label_str, category };
    }

    return true;
}

void TracepointMap::load_maps(const std::vector<std::string>& map_files)
{
    for (const auto& path : map_files)
    {
        if (!load_map(path))
        {
            std::cerr << "unable to load map file " << path << std::endl;
        }
    }
}

void TracepointMap::add_map_entry(uint64_t id, std::string label)
{
    MapInfo info = { label, category_to_string(Unknown) };
    _input_map[id] = info;
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
