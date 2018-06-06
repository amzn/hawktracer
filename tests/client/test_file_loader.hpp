#ifndef HT_TEST_CLIENT_TESTFILELOADER_HPP
#define HT_TEST_CLIENT_TESTFILELOADER_HPP

#include <client/call_graph.hpp>
#include <fstream>

using HawkTracer::client::CallGraph;

class TestFileLoader
{
public:
    bool init(std::string file_name); 
    std::vector<CallGraph::NodeData> get_events();
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> get_tree();

private:
    std::string _next_valid_line();
    void _read_tree_nodes();
    void _read_tree_edges();
    void _read_events_data();
    void _parse_file();

    std::ifstream _file;
    std::unordered_map<unsigned int, std::pair<std::shared_ptr<HawkTracer::client::CallGraph::TreeNode>, unsigned int>> _nodes;
    std::vector<CallGraph::NodeData> _events;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> _tree;
};

#endif //HT_TEST_CLIENT_TESTFILELOADER_HPP
