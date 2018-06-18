#ifndef HAWKTRACER_ANOMALY_JSON_TREES_FILE_LOADER_HPP
#define HAWKTRACER_ANOMALY_JSON_TREES_FILE_LOADER_HPP

#include <client/call_graph.hpp>
#include <thirdparty/jsonxx/jsonxx.h>

#include <fstream>

using HawkTracer::client::CallGraph;

namespace HawkTracer
{
namespace anomaly
{

class JsonTreesFileLoader
{
public:
    bool init(const std::string& file_name);
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> get_trees();
private:
    void _parse_file();
    void _parse_tree(const jsonxx::Value& tree);
    void _parse_node_data(const jsonxx::Value& node);
    void _parse_node_edges(const jsonxx::Value& node);
 
    std::ifstream _file;
    jsonxx::Object _json_obj;
    std::unordered_map<unsigned int, std::pair<std::shared_ptr<HawkTracer::client::CallGraph::TreeNode>, unsigned int>> _nodes;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> _trees;

};

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_JSON_TREES_FILE_LOADER_HPP
