#include "config.hpp"

#include <thirdparty/jsonxx/jsonxx.h>
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

unsigned int Config::get_relabel_cost()
{
    return _relabel_cost;
}

bool Config::get_ordered_tree()
{
    return _ordered_tree;
}

template<typename T, typename json_T>
static void try_get_value(const jsonxx::Object& json_obj, const std::string& field_name, T& field)
{
    if (json_obj.has<json_T>(field_name))
    {
        field = json_obj.get<json_T>(field_name);
    }
}

void Config::_load_file(std::ifstream& file)
{
    jsonxx::Object json_obj;
    json_obj.parse(file);
    try_get_value<unsigned int, jsonxx::Number>(json_obj, "insert_cost", _insert_cost);
    try_get_value<unsigned int, jsonxx::Number>(json_obj, "delete_cost", _delete_cost);
    try_get_value<unsigned int, jsonxx::Number>(json_obj, "relabel_cost", _relabel_cost);
    try_get_value<bool, jsonxx::Boolean>(json_obj, "ordered_tree", _ordered_tree);
}

} // namespace anomaly
} // namespace HawkTracer
