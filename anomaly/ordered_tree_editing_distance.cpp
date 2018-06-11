#ifndef HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_CPP
#define HAWKTRACER_ANOMALY_ORDERED_TREE_EDITING_DISTANCE_CPP

#include "ordered_tree_editing_distance.hpp"

namespace HawkTracer
{
namespace anomaly
{

OrderedTreeEditingDistance::OrderedTreeEditingDistance(std::shared_ptr<Config> config,
                                                       std::shared_ptr<TreeNode> src_tree,
                                                       std::shared_ptr<TreeNode> dst_tree) :
    _config(config),
    _src_tree(src_tree),
    _dst_tree(dst_tree)
{
}

int OrderedTreeEditingDistance::get()
{
    if (!_computed)
    {
        _result = _zhang_shasha_algorithm();
        _computed = true;
    }
    return _result;
}

static int _precompute(std::shared_ptr<TreeNode> node,
                       std::vector<std::string>& post_order,
                       std::vector<int>& left_most_leaf,
                       std::vector<int>& lr_key_roots)
{
    int left_most_child_pos;
    for (auto child = node->children.begin(); child != node->children.end(); ++child)
    {
        if (child == node->children.begin()) 
        {
            left_most_child_pos = _precompute(child->first, post_order, left_most_leaf, lr_key_roots);
        }
        else
        {
            int child_pos = _precompute(child->first, post_order, left_most_leaf, lr_key_roots);
            lr_key_roots.push_back(child_pos);
        }
    }

    post_order.push_back(node->data.label);
    if (node->children.size() == 0)
    {
        left_most_leaf.push_back(post_order.size() - 1);
    }
    else
    {
        left_most_leaf.push_back(left_most_leaf[left_most_child_pos]);
    }
    return post_order.size() - 1;
}

int OrderedTreeEditingDistance::_tree_dist(int src_node, 
                                           int dst_node,
                                           std::vector<std::vector<int>>& treedist,
                                           std::vector<std::vector<bool>>& computed)
{
    std::vector<std::vector<int>> forestdist(src_node + 2, std::vector<int>(dst_node + 2));

    forestdist[0][0] = 0;
    for (size_t i = _left_most_leaf_src[src_node]; i <= src_node; ++i)
    {
        forestdist[i + 1][0] = forestdist[i][0] + _config->get_delete_cost();
    }

    for (size_t j = _left_most_leaf_dst[dst_node]; j <= dst_node; ++j)
    {
        forestdist[0][j + 1] = forestdist[0][j] + _config->get_insert_cost();
    }

    for (size_t i = _left_most_leaf_src[src_node]; i <= src_node; ++i)
    {
        for (size_t j = _left_most_leaf_dst[dst_node]; j <= dst_node; ++j)
        {
            int prev_i = _left_most_leaf_src[src_node] <= i - 1 ? i - 1 : -1;
            int prev_j = _left_most_leaf_dst[dst_node] <= j - 1 ? j - 1 : -1;
            if (_left_most_leaf_src[i] == _left_most_leaf_src[src_node] &&
                _left_most_leaf_dst[j] == _left_most_leaf_dst[dst_node])
            {

                forestdist[i + 1][j + 1] = std::min(
                        forestdist[prev_i + 1][j + 1] + _config->get_delete_cost(),
               std::min(forestdist[i + 1][prev_j + 1] + _config->get_insert_cost(),
                        forestdist[prev_i + 1][prev_j + 1] + 
                        (_post_order_src[i] == _post_order_dst[j] ? 0 : _config->get_relabel_cost())));
                treedist[i][j] = forestdist[i + 1][j + 1];
                computed[i][j] = true;
            }
            else
            {
               
                int prev_left_most_leaf_src = _left_most_leaf_src[src_node] <= _left_most_leaf_src[i] - 1 ? _left_most_leaf_src[i] - 1 : -1;
                int prev_left_most_leaf_dst = _left_most_leaf_dst[dst_node] <= _left_most_leaf_dst[j] - 1 ? _left_most_leaf_dst[j] - 1 : -1;
                forestdist[i + 1][j + 1] = std::min(
                        forestdist[prev_i + 1][j + 1] + _config->get_delete_cost(),
               std::min(forestdist[i + 1][prev_j + 1] + _config->get_insert_cost(),
                        forestdist[prev_left_most_leaf_src + 1][prev_left_most_leaf_dst + 1] + treedist[i][j]));
            }
        }
    }
    return forestdist[src_node + 1][dst_node + 1];
}

int OrderedTreeEditingDistance::_zhang_shasha_algorithm()
{
    _precompute(_src_tree, _post_order_src, _left_most_leaf_src, _lr_key_roots_src);
    _precompute(_dst_tree, _post_order_dst, _left_most_leaf_dst, _lr_key_roots_dst);

    _lr_key_roots_src.push_back(_post_order_src.size() - 1);
    _lr_key_roots_dst.push_back(_post_order_dst.size() - 1);


    std::vector<std::vector<int>> treedist(_post_order_src.size(), std::vector<int>(_post_order_dst.size()));
    std::vector<std::vector<bool>> computed(_post_order_src.size(), std::vector<bool>(_post_order_dst.size(), false));
   
    printf("%d %d %d\n", _config->get_delete_cost(), _config->get_insert_cost(), _config->get_relabel_cost());
    for (size_t i = 0; i < _lr_key_roots_src.size(); ++i)
    {
        for (size_t j = 0; j < _lr_key_roots_dst.size(); ++j)
        {
            int src_node = _lr_key_roots_src[i];
            int dst_node = _lr_key_roots_dst[j];

            if (!computed[src_node][dst_node])
            {
                treedist[src_node][dst_node] = _tree_dist(src_node, dst_node, treedist, computed);
                computed[src_node][dst_node] = true;
            }
        }
    } 
    
    return treedist[_post_order_src.size() - 1][_post_order_dst.size() - 1];
}


} // namespace anomaly
} // namespace HawkTracer

#endif // HAWKTRACER_ANOMALY_ORDRED_TREE_EDITING_DISTANCE_CPP
