#ifndef HAWKTRACER_ANOMALY_PATTERNS_HPP
#define HAWKTRACER_ANOMALY_PATTERNS_HPP

#include "json_trees_file_loader.hpp"

#include <fstream>

namespace HawkTracer
{
namespace anomaly
{

class Graphs
{
public:
    bool load_from_file(const std::string& file_name);
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> get_trees();

private:
    JsonTreesFileLoader _file_loader;
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> _trees;
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_PATTERNS_HPP
