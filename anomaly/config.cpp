#include "config.hpp"
#include "jsonxx.h"

namespace HawkTracer
{
namespace anomaly
{

Config::Config() : 
    _insert_cost(0),
    _delete_cost(0),
    _relabel_cost(0),
    _ordered_tree(true)
{
}

bool Config::load_from_file(const std::string& file_name)
{
    _file.open(file_name);
    if (!_file.is_open())
    {
        return false;
    }
    _load_file();
    return true;
}

int Config::get_insert_cost()
{
    return _insert_cost;
}

int Config::get_delete_cost()
{
    return _delete_cost;
}

int Config::get_relabel_cost()
{
    return _relabel_cost;
}

bool Config::get_ordered_tree()
{
    return _ordered_tree;
}

template<typename T, typename json_T>
void Config::try_get_value(const std::string& field_name, T& field)
{
    if (_json_obj.has<json_T>(field_name))
    {
        field = _json_obj.get<json_T>(field_name);
    }
}

void Config::_load_file()
{
    _json_obj.parse(_file);
    try_get_value<int, jsonxx::Number>("insert_cost", _insert_cost);
    try_get_value<int, jsonxx::Number>("delete_cost", _delete_cost);
    try_get_value<int, jsonxx::Number>("relabel_cost", _relabel_cost);
    try_get_value<bool, jsonxx::Boolean>("ordered_tree", _ordered_tree);
}

} // namespace anomaly
} // namespace HawkTracer
