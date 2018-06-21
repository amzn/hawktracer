#include <anomaly/graphs.hpp>
#include <anomaly/ordered_tree_editing_distance.hpp>
#include <tests/client/test_path.hpp>

#include <gtest/gtest.h>

using HawkTracer::anomaly::OrderedTreeEditingDistance;

TEST(TestOrderedTreeEditingDistance, TestPostOrder)
{
    HawkTracer::anomaly::Graphs graphs;
    graphs.load_from_file(TestPath::get().get_input_file_path("test4_pattern.json"));
    const auto &trees = graphs.get_trees();
    ASSERT_TRUE(trees.size() == 1);
    
    std::vector<std::string> post_order;
    OrderedTreeEditingDistance::compute_post_order(trees[0].first, post_order);
    std::vector<std::string> correct_post_order = {"A", "B", "D", "C", "E", "F"};

    ASSERT_EQ(correct_post_order.size(), post_order.size());
    for (size_t i = 0; i < correct_post_order.size(); ++i)
    {
        ASSERT_EQ(correct_post_order[i], post_order[i]);
    }
}

TEST(TestPatternMatching, TestLeftMostLeaves)
{
    HawkTracer::anomaly::Graphs graphs;
    graphs.load_from_file(TestPath::get().get_input_file_path("test4_pattern.json"));
    const auto &trees = graphs.get_trees();
    ASSERT_TRUE(trees.size() == 1);
    
    std::vector<unsigned int> left_most_leaf;
    int index_last_node = -1;
    OrderedTreeEditingDistance::compute_left_most_leaves(trees[0].first, index_last_node, left_most_leaf);
    std::vector<unsigned int> correct_left_most_leaf = {0, 1, 0, 0, 4, 0}; 

    ASSERT_EQ(correct_left_most_leaf.size(), left_most_leaf.size());
    for (size_t i = 0; i < correct_left_most_leaf.size(); ++i)
    {
        ASSERT_EQ(correct_left_most_leaf[i], left_most_leaf[i]);
    }
}

TEST(TestPatternMatching, TestKeyRoots)
{
    HawkTracer::anomaly::Graphs graphs;
    graphs.load_from_file(TestPath::get().get_input_file_path("test4_pattern.json"));
    const auto &trees = graphs.get_trees();
    ASSERT_TRUE(trees.size() == 1);
    
    std::vector<unsigned int> key_roots;
    int index_last_node = -1;
    OrderedTreeEditingDistance::compute_key_roots(trees[0].first, index_last_node, key_roots);
    std::vector<unsigned int> correct_key_roots = {1, 4, 5};

    ASSERT_EQ(correct_key_roots.size(), key_roots.size());
    for (size_t i = 0; i < correct_key_roots.size(); ++i)
    {
        ASSERT_EQ(correct_key_roots[i], key_roots[i]);
    }
}

