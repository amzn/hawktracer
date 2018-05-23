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
    std::string label = _get_label(event);

    if (label == "")
    {
        return;
    }
    HT_ThreadId thread_id = event.get_value_or_default<HT_ThreadId>("thread_id", 0);
    HT_TimestampNs start_ts = event.get_value<HT_TimestampNs>("timestamp");
    HT_DurationNs duration = event.get_value_or_default<HT_DurationNs>("duration", 0u);
    _events.emplace_back(thread_id, CallGraph::TreeNode(label, start_ts, duration));
}

void CallgrindConverter::_print_function(std::ofstream& file, std::shared_ptr<CallGraph::TreeNode> root, std::string label)
{
    std::queue<std::pair<std::shared_ptr<CallGraph::TreeNode>, std::string>> fnc_queue;
    fnc_queue.emplace(root, root->label + "()");

    while (!fnc_queue.empty())
    {
        auto& fnc = fnc_queue.front();
        file << "fn=" << fnc.second << "\n";
        file << "1 " << fnc.first->duration - fnc.first->total_children_duration << "\n";
        for (const auto& child : fnc.first->children)
        {
            std::string child_label = child.first->label + "()'" + fnc.second;
            file << "cfn=" << child_label << "\n";
            file << "calls=" << child.second << " 1\n";
            file << "1 " << child.first->duration << "\n";
            fnc_queue.emplace(child.first, child_label);
        }
        fnc_queue.pop();
    }
}

void CallgrindConverter::stop()
{
    _call_graph.make(_events);
    for (auto& calls : _call_graph.root_calls)
    {
        std::ofstream thread_output_file(_file_name + "." + std::to_string(calls.first));
        if (thread_output_file.is_open())
        {
            thread_output_file << callgrind_header << std::endl;
            thread_output_file << "thread: " << calls.first << "\n\n";
            thread_output_file << "events: Duration" << "\n";

            for (auto& root : calls.second)
            {
                _print_function(thread_output_file, root.first, "");
            }

            thread_output_file.close();
        }
        else
        {
            std::cerr << "Can't open file: " << _file_name + "." + std::to_string(calls.first);
        }
    }
}

} // namespace client
} // namespace HawkTracer
