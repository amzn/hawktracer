#ifndef HAWKTRACER_ANOMALY_TREE_FILE_LOADER_HPP
#define HAWKTRACER_ANOMALY_TREE_FILE_LOADER_HPP

#include <client/call_graph.hpp>
#include <fstream>

using HawkTracer::client::CallGraph;

namespace HawkTracer
{
namespace anomaly
{

class TreeFileLoader
{
public:
    bool init(std::string file_name); 
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> get_trees();

protected:
    std::string _next_valid_line();
    void _read_tree_nodes();
    void _read_tree_edges();
    std::ifstream _file;
    std::unordered_map<unsigned int, std::pair<std::shared_ptr<HawkTracer::client::CallGraph::TreeNode>, unsigned int>> _nodes;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> _trees;

private:
    void _parse_file();
};

} // namespace anomaly
} // namespace HawkTracer

#endif //HAWKTRACER_ANOMALY_TREE_FILE_LOADER_HPP
