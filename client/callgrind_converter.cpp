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
    std::ofstream blank_file;
    blank_file.open(file_name);
    if (blank_file.is_open())
    {
        blank_file << callgrind_header << std::endl;
        blank_file.close();
        return true;
    }
    return false;
}


void CallgrindConverter::uninit()
{
}

void CallgrindConverter::_add_new_calltree(HT_ThreadId thread_id,
                                           std::string label,
                                           HT_TimestampNs start_ts,
                                           HT_DurationNs duration)
{
    auto& _root_calls_for_thread_id = _root_calls[thread_id];
    auto call = std::find_if(_root_calls_for_thread_id.begin(), _root_calls_for_thread_id.end(),
            [&label](const std::pair<TreeNode*, int>& call) {
            return call.first->label == label;
            });
    if (call != _root_calls_for_thread_id.end())
    {
        call->first->duration += duration;
        call->first->start_ts = start_ts;
        ++call->second;
    }
    else
    {
        _calls[thread_id] = new TreeNode(label, start_ts, duration);
        _root_calls_for_thread_id.emplace_back(_calls[thread_id], 1);
    }

}

void CallgrindConverter::_add_event(HT_ThreadId thread_id,
                                    std::string label,
                                    HT_TimestampNs start_ts,
                                    HT_DurationNs duration)
{
    auto thread_calls = _calls.find(thread_id);

    if (thread_calls == _calls.end())
    {
        _add_new_calltree(thread_id, label, start_ts, duration);
    }
    else
    {
        auto& previous_event = _calls[thread_id];

        while(previous_event != nullptr)
        {
            bool previous_event_contains_current_event = 
                previous_event->start_ts <= start_ts && start_ts + duration <= previous_event->get_stop_ts();

            if (previous_event_contains_current_event)
            {
                previous_event->total_children_duration += duration;

                auto call = std::find_if(previous_event->children.begin(), previous_event->children.end(),
                        [&label](const std::pair<TreeNode*, int>& child) {
                            return child.first->label == label;
                        });
                if (call != previous_event->children.end())
                {
                    call->first->duration += duration;
                    call->first->start_ts = start_ts;
                    ++call->second;
                    _calls[thread_id] = call->first;
                }
                else
                {
                    TreeNode* eventNode = new TreeNode(label, start_ts, duration);
                    eventNode->parent = previous_event;
                    previous_event->children.emplace_back(eventNode, 1);
                    _calls[thread_id] = eventNode;
                }

                break;
            }
            else
            {
               previous_event = previous_event->parent;
            }
        }
        if (previous_event == nullptr)
        {
            _add_new_calltree(thread_id, label, start_ts, duration);
        }
    }
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
    _events.emplace_back(thread_id, TreeNode(label, start_ts, duration));
}

void CallgrindConverter::_print_function(std::ofstream& file, TreeNode* node, std::string label)
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
    for (auto& child : node->children)
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
    for (auto it = _events.rbegin(); it != _events.rend(); ++it)
    {
        _add_event(it->first, it->second.label, it->second.start_ts, it->second.duration);
    }

    for (auto& calls : _root_calls)
    {
        std::ofstream thread_output_file(_file_name + "." + std::to_string(calls.first));
        if (thread_output_file.is_open())
        {
            thread_output_file<< "# callgrind format\n";
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
