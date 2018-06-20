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
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> get_trees();

private:
    std::string _next_valid_line();
    bool _read_tree_nodes();
    bool _read_tree_edges();
    bool _read_events_data();
    bool _parse_file();

    std::ifstream _file;
    std::unordered_map<unsigned int, std::pair<std::shared_ptr<HawkTracer::client::CallGraph::TreeNode>, unsigned int>> _nodes;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> _trees;
    std::vector<CallGraph::NodeData> _events;
};

#endif //HT_TEST_CLIENT_TESTFILELOADER_HPP
