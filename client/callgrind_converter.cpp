#include "callgrind_converter.hpp"

#include <algorithm>

namespace HawkTracer
{
namespace client
{

CallgrindConverter::~CallgrindConverter()
{
    uninit();
}

bool CallgrindConverter::init(const std::string& file_name)
{
    _file_name = file_name;
    return true;
}


void CallgrindConverter::uninit()
{
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

void CallgrindConverter::_print_function(std::ofstream& file, std::shared_ptr<CallGraph::TreeNode> node, std::string label)
{
    if (label != "")
    {
        label = node->label + "()'" + label;
    }
    else
    {
        label = node->label + "()";
    }
    file << "fn=" << label << "\n";
    file << "1 " << node->duration - node->total_children_duration << "\n";
    for (const auto& child : node->children)
    {
        std::string child_label = child.first->label + "()'" + label;
        file << "cfn=" << child_label << "\n";
        file << "calls=" << child.second << " 1\n";
        file << "1 " << child.first->duration << "\n";
    }
    for (auto& child : node->children)
    {
        _print_function(file, child.first, label);
    }
}

bool CallgrindConverter::stop()
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
            return false;
        }
    }
    uninit();
    return true;
}

} // namespace client
} // namespace HawkTracer
