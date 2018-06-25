#include "json_trees_file_loader.hpp"

#include <utils/json/jsonxx_utils.hpp>

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
    bool trees_exist = 
        HawkTracer::utils::get_value<jsonxx::Array, jsonxx::Array>(_json_obj, "trees", trees);
    if (!trees_exist)
    {
        return false;
    }

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

    bool label_read = 
        HawkTracer::utils::get_value<std::string, jsonxx::String>(*node.object_value_, "label", label);
    bool id_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "id", id);
    bool cnt_calls_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "cnt_calls", cnt_calls);
    bool last_start_ts_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "last_start_ts", last_start_ts);
    bool last_stop_ts_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "last_stop_ts", last_stop_ts);
    bool total_dur_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "total_dur", total_dur);
    bool total_children_dur_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "total_children_dur", total_children_dur);
    
    if (!label_read || !id_read || !cnt_calls_read || !last_start_ts_read || 
        !last_stop_ts_read || !total_dur_read || !total_children_dur_read)
    {
        return false;
    }

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

    bool id_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "id", id);
    bool parent_read = 
        HawkTracer::utils::get_value<int, jsonxx::Number>(*node.object_value_, "parent_id", parent_id);
    if (!id_read || !parent_read)
    {
        return false;
    }

    if (parent_id == 0)
    {
        _trees.emplace_back(_nodes[id].first, _nodes[id].second);
    }
    else
    {
        _nodes[id].first->parent = _nodes[parent_id].first;
    }

    jsonxx::Array children;
    HawkTracer::utils::get_value<jsonxx::Array, jsonxx::Array>(*node.object_value_, "children", children);
    for (const auto& child : children.values())
    {
        int child_id;
        int cnt_calls;
        bool child_id_read = 
            HawkTracer::utils::get_value<int, jsonxx::Number>(*child->object_value_, "id", child_id);
        bool child_cnt_calls_read = 
            HawkTracer::utils::get_value<int, jsonxx::Number>(*child->object_value_, "cnt_calls", cnt_calls);
        if (!child_id_read || !child_cnt_calls_read)
        {
            return false;
        }

        _nodes[id].first->children.emplace_back(_nodes[child_id].first, cnt_calls);
    }
    return true;
}

} // namespace anomaly
} // namespace HawkTracer
