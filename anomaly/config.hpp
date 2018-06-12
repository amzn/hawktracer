#ifndef HAWKTRACER_ANOMALY_CONFIG_HPP
#define HAWKTRACER_ANOMALY_CONFIG_HPP

#include "jsonxx.h"
#include <fstream>

namespace HawkTracer
{
namespace anomaly
{

class Config
{
public:
    Config();
    bool load_from_file(const std::string& file_name);
    int get_insert_cost();
    int get_delete_cost();
    int get_relabel_cost();
    bool get_ordered_tree();

private:
    jsonxx::Object _json_obj;
    std::ifstream _file;
    int _insert_cost;
    int _delete_cost;
    int _relabel_cost;
    bool _ordered_tree;

    void _load_file();
    template<typename T, typename json_T>
    void try_get_value(const std::string& field_name, T& field);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_CONFIG_HPP
