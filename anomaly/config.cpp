#include "config.hpp"

#include <thirdparty/jsonxx/jsonxx.h>
#include <utils/json/jsonxx_utils.hpp>
#include <fstream>

namespace HawkTracer
{
namespace anomaly
{

bool Config::load_from_file(const std::string& file_name)
{
    std::ifstream file;
    file.open(file_name);
    if (!file.is_open())
    {
        return false;
    }
    _load_file(file);
    return true;
}

unsigned int Config::get_insert_cost()
{
    return _insert_cost;
}

unsigned int Config::get_delete_cost()
{
    return _delete_cost;
}

unsigned int Config::get_relabel_cost(const std::string& src, const std::string& dst)
{
    if (src == dst)
    {
        return 0;
    }
    return _relabel_cost;
}

void Config::_load_file(std::ifstream& file)
{
    jsonxx::Object json_obj;
    json_obj.parse(file);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "insert_cost", _insert_cost);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "delete_cost", _delete_cost);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "relabel_cost", _relabel_cost);
}

} // namespace anomaly
} // namespace HawkTracer
