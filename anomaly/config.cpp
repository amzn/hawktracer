#include "config.hpp"
#include "jsonxx.h"

namespace HawkTracer
{
namespace anomaly
{

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

void Config::_load_file()
{
    jsonxx::Object json_obj;
    json_obj.parse(_file);
    
    _insert_cost = json_obj.get<jsonxx::Number>("insert_cost");
    _delete_cost = json_obj.get<jsonxx::Number>("delete_cost");
    if (json_obj.has<jsonxx::Number>("relabel_cost")) 
    {
        _relabel_cost = json_obj.get<jsonxx::Number>("relabel_cost");
    }
    else
    {
        _relabel_cost = _insert_cost + _delete_cost;
    }

    _ordered_tree = json_obj.get<jsonxx::Boolean>("ordered_tree");
}

} // namespace anomaly
} // namespace HawkTracer
