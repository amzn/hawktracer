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
    unsigned int get_relabel_cost(std::string& src, std::string& dst);

private:
    unsigned int _insert_cost = 0;
    unsigned int _delete_cost = 0;
    unsigned int _relabel_cost = 0;

    void _load_file(std::ifstream& file);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_CONFIG_HPP
