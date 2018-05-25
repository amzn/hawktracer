#include "test_path.hpp"
#include <client/call_graph.hpp>

#include <gtest/gtest.h>

#include <queue>
#include <fstream>
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
        auto& actual_fnc = actual_fnc_queue.front();
        auto& expected_fnc = expected_fnc_queue.front();

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
    std::ifstream file;
    file.open(file_name);
    if (file.is_open())
    {
        std::string line;
        std::unordered_map<unsigned int, std::pair<std::shared_ptr<CallGraph::TreeNode>, unsigned int>> nodes;

        // Read tree nodes
        size_t cnt_lines;
        file >> cnt_lines;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(file, line);

        // ID LABEL CNT_CALLS LAST_START_TS LAST_STOP_TS TOTAL_DUR TOTAL_CHILDREN_DUR
        for (size_t i = 0; i < cnt_lines; ++i) 
        {
            unsigned int id;
            std::string label;
            unsigned int cnt_calls;
            HT_TimestampNs last_start_ts;
            HT_TimestampNs last_stop_ts;
            HT_DurationNs total_dur;
            HT_DurationNs total_children_dur;

            file >> id >> label >> cnt_calls >> last_start_ts >> last_stop_ts >> total_dur >> total_children_dur; 

            std::shared_ptr<CallGraph::TreeNode> node = 
                std::make_shared<CallGraph::TreeNode>(CallGraph::NodeData(label, last_start_ts, last_stop_ts - last_start_ts));
            node->total_children_duration = total_children_dur;
            node->total_duration = total_dur;
            nodes[id] = make_pair(node, cnt_calls);

        }

        // Read tree edges
        file >> cnt_lines;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(file, line);

        // ID  PARENT_ID  CNT_CHILDREN (CHILD_ID, CNT_CALLS)
        for (size_t i = 0; i < cnt_lines; ++i)
        {
            unsigned int id;
            unsigned int parent_id;
            unsigned int cnt_children;

            file >> id >> parent_id >> cnt_children;

            if (parent_id == 0)
            {
                tree.emplace_back(nodes[id].first, nodes[id].second);
            }

            else
            {
                nodes[id].first->parent = nodes[parent_id].first;
            }
            for (size_t i = 0; i < cnt_children; ++i)
            {
                unsigned int child_id;
                unsigned int cnt_calls;

                file >> child_id >> cnt_calls;
                nodes[id].first->children.emplace_back(nodes[child_id].first, cnt_calls);
            }

        }

        // Read events data
        file >> cnt_lines;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(file, line);

        // LABEL   START_TS   DURATION
        for (size_t i = 0; i < cnt_lines; ++i)
        { 
            std::string label;
            HT_TimestampNs start_ts = 0;
            HT_DurationNs dur = 0;

            file >> label >> start_ts >> dur;

            events.emplace_back(label, start_ts, dur);
        }
    }

    else
    {
        std::cout << "File not open\n";
    }
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


