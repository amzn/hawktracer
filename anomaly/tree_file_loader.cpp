#ifndef HAWKTRACER_ANOMALY_TREE_FILE_LOADER_CPP
#define HAWKTRACER_ANOMALY_TREE_FILE_LOADER_CPP

#include "tree_file_loader.hpp"
#include <sstream>

namespace HawkTracer
{
namespace anomaly
{

bool TreeFileLoader::init(std::string file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        _parse_file();
    }
    return _file.is_open();
}

std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> TreeFileLoader::get_trees()
{
    return _trees;
}

std::string TreeFileLoader::_next_valid_line()
{
    std::string line;
    while (std::getline(_file, line))
    {
        if (!(line[0] == '#' || line.find_first_not_of(" \n") == std::string::npos))
        {
            break;
        }
    }
    return line;
}

void TreeFileLoader::_read_tree_nodes()
{
    size_t cnt_lines = stoi(_next_valid_line());

    // ID LABEL CNT_CALLS LAST_START_TS LAST_STOP_TS TOTAL_DUR TOTAL_CHILDREN_DUR
    for (size_t i = 0; i < cnt_lines; ++i) 
    {
        std::stringstream line_stream(_next_valid_line());
        unsigned int id;
        std::string label;
        unsigned int cnt_calls;
        HT_TimestampNs last_start_ts;
        HT_TimestampNs last_stop_ts;
        HT_DurationNs total_dur;
        HT_DurationNs total_children_dur;

        line_stream >> id >> label >> cnt_calls >> last_start_ts >> last_stop_ts >> total_dur >> total_children_dur; 

        std::shared_ptr<CallGraph::TreeNode> node = 
            std::make_shared<CallGraph::TreeNode>(CallGraph::NodeData(label, last_start_ts, last_stop_ts - last_start_ts));
        node->total_children_duration = total_children_dur;
        node->total_duration = total_dur;
        _nodes[id] = make_pair(node, cnt_calls);
    }
}

void TreeFileLoader::_read_tree_edges()
{
    size_t cnt_lines = stoi(_next_valid_line());

    // ID  PARENT_ID  CNT_CHILDREN (CHILD_ID, CNT_CALLS)
    for (size_t i = 0; i < cnt_lines; ++i)
    {
        std::stringstream line_stream(_next_valid_line());
        unsigned int id;
        unsigned int parent_id;
        unsigned int cnt_children;

        line_stream >> id >> parent_id >> cnt_children;

        if (parent_id == 0)
        {
            _trees.emplace_back(_nodes[id].first, _nodes[id].second);
        }
        else
        {
            _nodes[id].first->parent = _nodes[parent_id].first;
        }
        for (size_t i = 0; i < cnt_children; ++i)
        {
            unsigned int child_id;
            unsigned int cnt_calls;

            line_stream >> child_id >> cnt_calls;
            _nodes[id].first->children.emplace_back(_nodes[child_id].first, cnt_calls);
        }
    }
}

void TreeFileLoader::_parse_file()
{
    _read_tree_nodes();
    _read_tree_edges();
}

} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_TREE_FILE_LOADER_CPP

