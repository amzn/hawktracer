#include "test_path.hpp"
#include "test_file_loader.hpp"
#include <client/call_graph.hpp>

#include <gtest/gtest.h>

#include <queue>
#include <iostream>

using HawkTracer::client::CallGraph;

::testing::AssertionResult SameTree(const std::pair<std::shared_ptr<CallGraph::TreeNode>, int>& expected, 
                                    const std::pair<std::shared_ptr<CallGraph::TreeNode>, int>& actual)
{
    if (expected.second != actual.second)
    {
        return ::testing::AssertionFailure() << std::endl
            << "Expected calls to root: " << expected.second << std::endl
            << "Actual calls to root: " << actual.second << std::endl;
    }

    std::queue<std::shared_ptr<CallGraph::TreeNode>> actual_fnc_queue;
    std::queue<std::shared_ptr<CallGraph::TreeNode>> expected_fnc_queue;
    actual_fnc_queue.push(actual.first); 
    expected_fnc_queue.push(expected.first);

    while (!actual_fnc_queue.empty())
    {
        const auto& actual_fnc = actual_fnc_queue.front();
        const auto& expected_fnc = expected_fnc_queue.front();

        if (expected_fnc->data.label != actual_fnc->data.label)
        {
            return ::testing::AssertionFailure() << std::endl 
                << "Expected label: " << expected_fnc->data.label << std::endl
                << "Actual label: " << actual_fnc->data.label << std::endl;
        }
        if (expected_fnc->data.start_ts != actual_fnc->data.start_ts)
        {
            return ::testing::AssertionFailure() << std::endl 
                << "For node with label: " << expected_fnc->data.label << std::endl
                << "Expected start_ts: " << expected_fnc->data.start_ts << std::endl
                << "Actual start_ts: " << actual_fnc->data.start_ts << std::endl;
        }
        if (expected_fnc->data.stop_ts  != actual_fnc->data.stop_ts)
        {
            return ::testing::AssertionFailure() << std::endl 
                << "For node with label: " << expected_fnc->data.label << std::endl
                << "Expected stop_ts: " << expected_fnc->data.stop_ts << std::endl
                << "Actual stop_ts: " << actual_fnc->data.stop_ts << std::endl;
        }
        
        if (expected_fnc->children.size() != actual_fnc->children.size())
        {
            return ::testing::AssertionFailure() << std::endl 
                << "For node with label: " << expected_fnc->data.label << std::endl
                << "Expected number of children: " << expected_fnc->children.size() << std::endl
                << "Actual number of children: " << actual_fnc->children.size() << std::endl;
        }

        for (size_t i = 0; i < actual_fnc->children.size(); ++i)
        {
            if (expected_fnc->children[i].second != actual_fnc->children[i].second) 
            {
                return ::testing::AssertionFailure() << std::endl
                    << "For node with label: " << expected_fnc->data.label << std::endl
                    << "Expected number of calls to a child: " << expected_fnc->children[i].second << std::endl
                    << "Actual number of calls to a child: " << actual_fnc->children[i].second << std::endl;
            }

            actual_fnc_queue.push(actual_fnc->children[i].first);
            expected_fnc_queue.push(expected_fnc->children[i].first);
        }

        actual_fnc_queue.pop();
        expected_fnc_queue.pop();
    }

    return ::testing::AssertionSuccess();
}

void init(std::vector<CallGraph::NodeData>& events,
          std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>>& tree,
          std::string file_name)
{
    TestFileLoader file_loader;
    ASSERT_TRUE(file_loader.init(file_name));
    tree = file_loader.get_tree();
    events = file_loader.get_events();
}

TEST(TestCallGraph, EmptyVectorOfEventShouldReturnEmptyGraph)
{
    // Arrange
    std::vector<CallGraph::NodeData> events;
    CallGraph call_graph;

    // Act
    auto response = call_graph.make(events);

    // Assert
    ASSERT_TRUE(response.empty());
}


TEST(TestCallGraph, Test3LevelsCallStackWithSimpleCalls)
{
    // Arrange
    std::vector<CallGraph::NodeData> events;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> correct_response;
    init(events, correct_response, TestPath::get().get_input_file_path("test_3_lvls_stack_simple_calls.txt"));

    CallGraph call_graph;

    // Act
    auto response = call_graph.make(events);

    // Assert
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < correct_response.size(); ++i)
    {
        ASSERT_TRUE(SameTree(correct_response[i], response[i]));
    }
}

TEST(TestCallGraph, TestMultpleCalls)
{
    // Arrange
    std::vector<CallGraph::NodeData> events;
    std::vector<std::pair<std::shared_ptr<CallGraph::TreeNode>, int>> correct_response;
    init(events, correct_response, TestPath::get().get_input_file_path("test_multiple_calls.txt"));

    CallGraph call_graph;

    // Act
    auto response = call_graph.make(events);

    // Assert
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < correct_response.size(); ++i)
    {
        ASSERT_TRUE(SameTree(correct_response[i], response[i]));
    }
}


