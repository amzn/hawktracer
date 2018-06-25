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

unsigned int Config::get_relabel_cost()
{
    return _relabel_cost;
}

unsigned int Config::get_relabel_cost(const std::string& src, const std::string& dst)
{
    if (src == dst)
    {
        return 0;
    }
    return _relabel_cost;
}

bool Config::get_consider_dur()
{
    return _consider_dur;
}

bool Config::get_consider_children_prop()
{
    return _consider_children_prop;
}

bool Config::get_consider_relative_start_time()
{
    return _consider_relative_start_time;
}

double Config::get_anomaly_score_threshold()
{
    return _anomaly_score_threshold;
}

unsigned int Config::get_max_insertions()
{
    return _max_insertions;
}

unsigned int Config::get_max_deletions()
{
    return _max_deletions;
}

unsigned int Config::get_max_relable()
{
    return _max_relable;
}

void Config::_load_file(std::ifstream& file)
{
    jsonxx::Object json_obj;
    json_obj.parse(file);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "insert_cost", _insert_cost);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "delete_cost", _delete_cost);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "relabel_cost", _relabel_cost);
    HawkTracer::utils::get_value<bool, jsonxx::Boolean>(json_obj, "consider_dur", _consider_dur);
    HawkTracer::utils::get_value<bool, jsonxx::Boolean>(json_obj, "consider_children_prop", _consider_children_prop);
    HawkTracer::utils::get_value<bool, jsonxx::Boolean>(json_obj, "consider_relative_start_time", _consider_relative_start_time);
    HawkTracer::utils::get_value<double, jsonxx::Number>(json_obj, "anomaly_score_threshold", _anomaly_score_threshold);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "max_insertions", _max_insertions);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "max_deletions", _max_deletions);
    HawkTracer::utils::get_value<unsigned int, jsonxx::Number>(json_obj, "max_relable", _max_relable);
}

} // namespace anomaly
} // namespace HawkTracer
