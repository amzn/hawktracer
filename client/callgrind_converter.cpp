#include "callgrind_converter.hpp"

#include <algorithm>
#include <iostream>
#include <queue>

namespace HawkTracer
{
namespace client
{

CallgrindConverter::~CallgrindConverter()
{
}

bool CallgrindConverter::init(const std::string& file_name)
{
    _file_name = file_name;
    return true;
}

void CallgrindConverter::process_event(const parser::Event& event)
{
    std::string label = _label_mapping->process_event(event);

    if (label == "")
    {
        return;
    }
    HT_ThreadId thread_id = event.get_value_or_default<HT_ThreadId>("thread_id", 0u);
    HT_TimestampNs start_ts = event.get_timestamp();
    HT_DurationNs duration = event.get_value_or_default<HT_DurationNs>("duration", 0u);
    _events[thread_id].emplace_back(label, start_ts, duration);
}

void CallgrindConverter::_print_function(std::ofstream& file, std::shared_ptr<CallGraph::TreeNode> root)
{
    std::queue<std::pair<std::shared_ptr<CallGraph::TreeNode>, std::string>> fnc_queue;
    fnc_queue.emplace(root, root->data.label + "()");

    while (!fnc_queue.empty())
    {
        auto& fnc = fnc_queue.front();
        file << "fn=" << fnc.second << "\n";
        file << "1 " << fnc.first->total_duration - fnc.first->total_children_duration << "\n";
        for (const auto& child : fnc.first->children)
        {
            std::string child_label = child.first->data.label + "()'" + fnc.second;
            file << "cfn=" << child_label << "\n";
            file << "calls=" << child.second << " 1\n";
            file << "1 " << child.first->total_duration << "\n";
            fnc_queue.emplace(child.first, child_label);
        }
        fnc_queue.pop();
    }
}

void CallgrindConverter::stop()
{
    for (auto& thread : _events)
    {
        CallGraph _call_graph;
        auto root_calls = _call_graph.make(thread.second);

        std::string thread_file_name = _file_name + "." + std::to_string(thread.first);
        std::ofstream thread_output_file(thread_file_name);
        if (thread_output_file.is_open())
        {
            thread_output_file << _callgrind_header << std::endl;
            thread_output_file << "thread: " << thread.first << "\n\n";
            thread_output_file << "events: Duration" << "\n";
            for (auto& root : root_calls)
            {
                _print_function(thread_output_file, root.first);
            }
        }
        else
        {
            std::cerr << "Can't open file: " << thread_file_name << std::endl;
        }
        thread_output_file.close();
    }
}

} // namespace client
} // namespace HawkTracer
