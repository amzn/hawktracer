#ifndef HAWKTRACER_ANOMALY_SOURCE_HPP
#define HAWKTRACER_ANOMALY_SOURCE_HPP

#include <client/call_graph.hpp>

#include "tree_file_loader.hpp"

#include <vector>

namespace HawkTracer
{
namespace anomaly
{

class Source
{
public:
    bool load_from_file(const std::string& file_name);
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> get_trees();

private:
    TreeFileLoader _file_loader;
    std::vector<std::pair<std::shared_ptr<client::CallGraph::TreeNode>, int>> _trees;
};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_SOURCE_HPP
