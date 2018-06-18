#ifndef HAWKTRACER_ANOMALY_CONFIG_HPP
#define HAWKTRACER_ANOMALY_CONFIG_HPP

#include <thirdparty/jsonxx/jsonxx.h>
#include <fstream>

namespace HawkTracer
{
namespace anomaly
{

class Config
{
public:
    bool load_from_file(const std::string& file_name);
    unsigned int get_insert_cost();
    unsigned int get_delete_cost();
    unsigned int get_relabel_cost();
    bool get_ordered_tree();

private:
    jsonxx::Object _json_obj;
    std::ifstream _file;
    unsigned int _insert_cost = 0;
    unsigned int _delete_cost = 0;
    unsigned int _relabel_cost = 0;
    bool _ordered_tree = true;

    void _load_file();
    template<typename T, typename json_T>
    void try_get_value(const std::string& field_name, T& field);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_CONFIG_HPP
