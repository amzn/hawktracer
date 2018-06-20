#include "json_trees_file_loader.hpp"

#include <thirdparty/jsonxx/jsonxx_utils.h>

namespace HawkTracer
{
namespace anomaly
{

bool JsonTreesFileLoader::init(const std::string& file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        return _parse_file();
    }
    return false;
}

std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> JsonTreesFileLoader::get_trees()
{
    return _trees;
}

bool JsonTreesFileLoader::_parse_file()
{
    _json_obj.parse(_file);
    jsonxx::Array trees;
    try_get_value<jsonxx::Array, jsonxx::Array>(_json_obj, "trees", trees);
    for (const auto& tree : trees.values())
    {
        if (!_parse_tree(*tree))
        {
            return false;
        }
    }
    return true;
}

bool JsonTreesFileLoader::_parse_tree(const jsonxx::Value& tree)
{
    for (const auto& node : tree.array_value_->values())
    {
        if (!_parse_node_data(*node))
        {
            return false;
        }
    } 
    for (const auto& node : tree.array_value_->values())
    {
        if (!_parse_node_edges(*node))
        {
            return false;
        }
    }   
    return true;
}

bool JsonTreesFileLoader::_parse_node_data(const jsonxx::Value& node)
{
    int id;
    int cnt_calls;
    int last_start_ts;
    int last_stop_ts;
    int total_dur;
    int total_children_dur;
    std::string label;

    if (!node.object_value_->has<jsonxx::String>("label") || !node.object_value_->has<jsonxx::Number>("id"))
    {
        return false;
    }
    try_get_value<std::string, jsonxx::String>(*node.object_value_, "label", label);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "id", id);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "cnt_calls", cnt_calls);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "last_start_ts", last_start_ts);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "last_stop_ts", last_stop_ts);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "total_dur", total_dur);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "total_children_dur", total_children_dur);

    auto tree_node = std::make_shared<CallGraph::TreeNode>(CallGraph::NodeData(label, last_start_ts, last_stop_ts - last_start_ts));
    tree_node->total_children_duration = total_children_dur;
    tree_node->total_duration = total_dur;
    _nodes[id] = make_pair(tree_node, cnt_calls);

    return true;
}

bool JsonTreesFileLoader::_parse_node_edges(const jsonxx::Value& node)
{
    int parent_id;
    int id;

    if (!node.object_value_->has<jsonxx::Number>("id") || !node.object_value_->has<jsonxx::Number>("parent_id"))
    {
        return false;
    }

    try_get_value<int, jsonxx::Number>(*node.object_value_, "id", id);
    try_get_value<int, jsonxx::Number>(*node.object_value_, "parent_id", parent_id);
    if (parent_id == 0)
    {
        _trees.emplace_back(_nodes[id].first, _nodes[id].second);
    }
    else
    {
        _nodes[id].first->parent = _nodes[parent_id].first;
    }

    jsonxx::Array children;
    try_get_value<jsonxx::Array, jsonxx::Array>(*node.object_value_, "children", children);
    for (const auto& child : children.values())
    {
        int child_id;
        int cnt_calls;
        try_get_value<int, jsonxx::Number>(*child->object_value_, "id", child_id);
        try_get_value<int, jsonxx::Number>(*child->object_value_, "cnt_calls", cnt_calls);

        _nodes[id].first->children.emplace_back(_nodes[child_id].first, cnt_calls);
    }
    return true;
}

} // namespace anomaly
} // namespace HawkTracer
