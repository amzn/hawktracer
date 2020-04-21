#ifndef HAWKTRACER_CLIENT_TRACEPOINT_MAP_HPP
#define HAWKTRACER_CLIENT_TRACEPOINT_MAP_HPP

#include <string>
#include <unordered_map>

namespace HawkTracer
{
namespace ClientUtils
{

class TracepointMap
{
public:
    enum Category : uint32_t
    {
        Unknown = 0,
        Lua = 1,
        Native = 2,
    };

    struct MapInfo
    {
        std::string label;
        std::string category;
    };

    MapInfo get_label_info(uint64_t label);

    bool load_map(const std::string& map_file);
    void load_maps(const std::string& map_files);

    void add_map_entry(uint64_t id, std::string label);

private:
    std::unordered_map<uint64_t, MapInfo> _input_map;
    static std::string category_to_string(Category category);
};

} // ClientUtils
} // HawkTracer

#endif // HAWKTRACER_CLIENT_TRACEPOINT_MAP_HPP
