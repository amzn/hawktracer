#include "ordered_tree_editing_distance.hpp"

using Mapping = std::vector<std::pair<unsigned int, unsigned int>>;
using OperationType = HawkTracer::anomaly::OrderedTreeEditingDistance::OperationType;

namespace HawkTracer
{
namespace anomaly
{

OrderedTreeEditingDistance::OrderedTreeEditingDistance(std::shared_ptr<Config> config,
                                                       std::shared_ptr<TreeNode> src_tree,
                                                       std::shared_ptr<TreeNode> dst_tree) :
    _config(std::move(config)),
    _src_tree(std::move(src_tree)),
    _dst_tree(std::move(dst_tree))
{
}

unsigned int OrderedTreeEditingDistance::get_distance()
{
    if (!_computed)
    {
        _result = _zhang_shasha_algorithm(nullptr);
        _computed = true;
    }
    return _result;
}

Mapping OrderedTreeEditingDistance::get_mapping()
{
    auto optimal_mapping = std::make_shared<Mapping>();
    _zhang_shasha_algorithm(optimal_mapping);
    return *optimal_mapping;
}

void OrderedTreeEditingDistance::_compute_key_roots(std::shared_ptr<TreeNode> node,
                                                    int& index_last_node,
                                                    std::vector<unsigned int>& lr_key_roots)
{
    for (auto child = node->children.begin(); child != node->children.end(); ++child)
    {
        if (child == node->children.begin()) 
        {
            _compute_key_roots(child->first, index_last_node, lr_key_roots);
        }
        else
        {
            _compute_key_roots(child->first, index_last_node, lr_key_roots);
            lr_key_roots.push_back(index_last_node);
        }
    }
    ++index_last_node;
}

void OrderedTreeEditingDistance::compute_key_roots(std::shared_ptr<TreeNode> root,
                                                   int& index_last_node,
                                                   std::vector<unsigned int>& lr_key_roots)
{
    _compute_key_roots(root, index_last_node, lr_key_roots);
    lr_key_roots.push_back(index_last_node);
}



void OrderedTreeEditingDistance::compute_post_order(std::shared_ptr<TreeNode> node,
                                                    std::vector<std::string>& post_order)
{
    for (auto child = node->children.begin(); child != node->children.end(); ++child)
    {
        compute_post_order(child->first, post_order);
    }

    post_order.push_back(node->data.label);
}

int OrderedTreeEditingDistance::compute_left_most_leaves(std::shared_ptr<TreeNode> node,
                                                         int& index_last_node,
                                                         std::vector<unsigned int>& left_most_leaf)
{
    int left_most_child_pos;
    for (auto child = node->children.begin(); child != node->children.end(); ++child)
    {
        if (child == node->children.begin()) 
        {
            left_most_child_pos = compute_left_most_leaves(child->first, index_last_node, left_most_leaf);
        }
        else
        {
            compute_left_most_leaves(child->first, index_last_node, left_most_leaf);
        }
    }

    ++index_last_node;
    if (node->children.empty())
    {
        left_most_leaf.push_back(index_last_node);
    }
    else
    {
        left_most_leaf.push_back(left_most_leaf[left_most_child_pos]);
    }
    return index_last_node;
}

unsigned int OrderedTreeEditingDistance::_get_relabel_cost(const std::string& src, const std::string& dst)
{
    return src == dst ? 0 : _config->get_relabel_cost();
}

// implementation of zhang and shasha algorithm for finding the editing distance
// between two trees. original paper:
// http://www.grantjenks.com/wiki/_media/ideas:simple_fast_algorithms_for_the_editing_distance_between_tree_and_related_problems.pdf

unsigned int OrderedTreeEditingDistance::_zhang_shasha_algorithm(std::shared_ptr<std::vector<std::pair<unsigned int, unsigned int>>> optimal_mapping)
{
    compute_post_order(_src_tree, _post_order_src);
    int index_last_node = -1;
    compute_left_most_leaves(_src_tree, index_last_node, _left_most_leaf_src);
    index_last_node = -1;
    compute_key_roots(_src_tree, index_last_node, _lr_key_roots_src);

    compute_post_order(_dst_tree, _post_order_dst);
    index_last_node = -1;
    compute_left_most_leaves(_dst_tree, index_last_node, _left_most_leaf_dst);
    index_last_node = -1;
    compute_key_roots(_dst_tree, index_last_node, _lr_key_roots_dst);

    std::vector<std::vector<int>> treedist(_post_order_src.size(), std::vector<int>(_post_order_dst.size(), -1));
    std::shared_ptr<std::vector<std::vector<Mapping>>> mapping;
    if (optimal_mapping != nullptr)
    {
        mapping = std::make_shared<std::vector<std::vector<Mapping>>>(_post_order_src.size(), std::vector<Mapping>(_post_order_dst.size()));
    }
   
    for (size_t i = 0; i < _lr_key_roots_src.size(); ++i)
    {
        for (size_t j = 0; j < _lr_key_roots_dst.size(); ++j)
        {
            size_t src_node = _lr_key_roots_src[i];
            size_t dst_node = _lr_key_roots_dst[j];

            if (treedist[src_node][dst_node] == -1)
            {
                treedist[src_node][dst_node] = _tree_dist(src_node, dst_node, treedist, mapping);
            }
        }
    }   
    if (optimal_mapping != nullptr)
    {
        *optimal_mapping = (*mapping)[_post_order_src.size() - 1][_post_order_dst.size() - 1];
    }
    return treedist[_post_order_src.size() - 1][_post_order_dst.size() - 1];
}

unsigned int OrderedTreeEditingDistance::_tree_dist(unsigned int src_node, 
                                                    unsigned int dst_node,
                                                    std::vector<std::vector<int>>& treedist,
                                                    std::shared_ptr<std::vector<std::vector<Mapping>>> mapping)
{
    std::vector<std::vector<unsigned int>> forestdist(src_node + 2, std::vector<unsigned int>(dst_node + 2));
    std::vector<std::vector<OperationType>> forestoperation;
    if (mapping)
    {
        forestoperation = std::vector<std::vector<OperationType>>(src_node + 2, std::vector<OperationType>(dst_node + 2));
    }

    forestdist[0][0] = 0;
    for (size_t i = _left_most_leaf_src[src_node]; i <= src_node; ++i)
    {
        forestdist[i + 1][0] = forestdist[i][0] + _config->get_delete_cost();
        if (mapping)
        {
            forestoperation[i + 1][0] = OperationType::DELETE;
        }
    }

    for (size_t j = _left_most_leaf_dst[dst_node]; j <= dst_node; ++j)
    {
        forestdist[0][j + 1] = forestdist[0][j] + _config->get_insert_cost();
        if (mapping)
        {
            forestoperation[0][j + 1] = OperationType::INSERT;
        }
    }

    for (size_t i = _left_most_leaf_src[src_node]; i <= src_node; ++i)
    {
        for (size_t j = _left_most_leaf_dst[dst_node]; j <= dst_node; ++j)
        {
            size_t prev_i = _left_most_leaf_src[src_node] <= i - 1 ? i : 0;
            size_t prev_j = _left_most_leaf_dst[dst_node] <= j - 1 ? j : 0;
            if (_left_most_leaf_src[i] == _left_most_leaf_src[src_node] &&
                _left_most_leaf_dst[j] == _left_most_leaf_dst[dst_node])
            {

                // Delete
                unsigned int delete_cost = forestdist[prev_i][j + 1] + _config->get_delete_cost();

                // Insert
                unsigned int insert_cost = forestdist[i + 1][prev_j] + _config->get_insert_cost();

                // Relabel or Match
                unsigned int relabel_cost = forestdist[prev_i][prev_j] + _get_relabel_cost(_post_order_src[i], _post_order_dst[j]);

                forestdist[i + 1][j + 1] = std::min(delete_cost, std::min(insert_cost, relabel_cost));
                if (mapping)
                {
                    if (forestdist[i + 1][j + 1] == delete_cost)
                    {
                        forestoperation[i + 1][j + 1]= OperationType::DELETE;
                    }
                    else if (forestdist[i + 1][j + 1] == insert_cost)
                    {
                        forestoperation[i + 1][j + 1] = OperationType::INSERT;
                    }
                    else if (_post_order_src[i] == _post_order_dst[j])
                    {
                        forestoperation[i + 1][j + 1] = OperationType::MATCH;
                    }
                    else
                    {
                        forestoperation[i + 1][j + 1] = OperationType::RELABEL;
                    }
                }

                treedist[i][j] = forestdist[i + 1][j + 1];
                if (mapping && !(i == src_node && j == dst_node))
                {
                    _compute_mapping(i, j, forestoperation, *mapping);
                }
            }
            else
            {
                size_t prev_left_most_leaf_src = _left_most_leaf_src[src_node] <= _left_most_leaf_src[i] - 1 ? _left_most_leaf_src[i] : 0;
                size_t prev_left_most_leaf_dst = _left_most_leaf_dst[dst_node] <= _left_most_leaf_dst[j] - 1 ? _left_most_leaf_dst[j] : 0;

                // Delete
                unsigned int delete_cost = forestdist[prev_i][j + 1] + _config->get_delete_cost();

                // Insert
                unsigned int insert_cost = forestdist[i + 1][prev_j] + _config->get_insert_cost();

                // Subtree match
                unsigned int subtree_match = forestdist[prev_left_most_leaf_src][prev_left_most_leaf_dst] + treedist[i][j];

                forestdist[i + 1][j + 1] = std::min(delete_cost, std::min(insert_cost, subtree_match));
                if (mapping)
                {
                    if (forestdist[i + 1][j + 1] == delete_cost)
                    {
                        forestoperation[i + 1][j + 1] = OperationType::DELETE;
                    }
                    else if (forestdist[i + 1][j + 1] == insert_cost)
                    {
                        forestoperation[i + 1][j + 1] = OperationType::INSERT;
                    }
                    else if (forestdist[i + 1][j + 1] == subtree_match)
                    {
                        forestoperation[i + 1][j + 1] = OperationType::SUBTREEMATCH;
                    }
                }
            }
        }
    }

    if (mapping)
    {
        _compute_mapping(src_node, dst_node, forestoperation, *mapping); 
    }
    return forestdist[src_node + 1][dst_node + 1];
}

void OrderedTreeEditingDistance::_compute_mapping(unsigned int src_node,
                                                  unsigned int dst_node,
                                                  std::vector<std::vector<OperationType>>& forestoperation,
                                                  std::vector<std::vector<Mapping>>& mapping)
{
    int i = src_node;
    int j = dst_node;
    while (i >= 0 && j >= 0)
    {
        int prev_i = _left_most_leaf_src[src_node] <= i - 1 ? i - 1 : -1;
        int prev_j = _left_most_leaf_dst[dst_node] <= j - 1 ? j - 1 : -1;

        switch (forestoperation[i + 1][j + 1])
        {
            case OperationType::DELETE:
                i = prev_i;
                break;
            case OperationType::INSERT:
                j = prev_j;
                break;
            case OperationType::RELABEL:
            case OperationType::MATCH:
                mapping[src_node][dst_node].emplace_back(i, j);
                i = prev_i;
                j = prev_j;
                break;
            case OperationType::SUBTREEMATCH:
                mapping[src_node][dst_node].insert(mapping[src_node][dst_node].end(), mapping[i][j].begin(), mapping[i][j].end());

                int prev_left_most_leaf_src = _left_most_leaf_src[src_node] <= _left_most_leaf_src[i] - 1 ? _left_most_leaf_src[i] - 1 : -1;
                int prev_left_most_leaf_dst = _left_most_leaf_dst[dst_node] <= _left_most_leaf_dst[j] - 1 ? _left_most_leaf_dst[j] - 1 : -1;

                i = prev_left_most_leaf_src;
                j = prev_left_most_leaf_dst;
                break;
        }
    }
}

} // namespace anomaly
} // namespace HawkTracer
