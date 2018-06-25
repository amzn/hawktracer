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

    // Conditions for checking anomalies
    bool get_consider_dur();
    bool get_consider_children_prop();
    bool get_consider_relative_start_time();

    // Number between 0 and 1.
    // Every score above this value is considered an anomaly
    double get_anomaly_score_threshold();

    // Maximum number or insert, delete, relable operations
    // allowed in searching patterns
    unsigned int get_max_insertions();
    unsigned int get_max_deletions();
    unsigned int get_max_relabel();

private:
    unsigned int _insert_cost = 0;
    unsigned int _delete_cost = 0;
    unsigned int _relabel_cost = 0;
    bool _consider_dur = true;
    bool _consider_children_prop = true;
    bool _consider_relative_start_time = true;
    double _anomaly_score_threshold = 0.1;
    unsigned int _max_insertions = 4;
    unsigned int _max_deletions = 4;
    unsigned int _max_relabel = 4;

    void _load_file(std::ifstream& file);
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_CONFIG_HPP
