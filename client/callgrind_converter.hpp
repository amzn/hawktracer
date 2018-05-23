#ifndef HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "converter.hpp"
#include "tracepoint_map.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

class CallgrindConverter : public Converter
{
public:
    ~CallgrindConverter() override;

    bool init(const std::string& file_name) override;
    void uninit() override;
    void process_event(const parser::Event& event) override;
    bool stop() override;

private:
    struct TreeNode
    {
        std::string label;
        HT_TimestampNs start_ts;
        HT_DurationNs duration;
        std::vector<std::pair<TreeNode*, int>> children;
        TreeNode* parent;
        HT_DurationNs total_children_duration;

        TreeNode(std::string name, HT_TimestampNs start, HT_DurationNs dur)
        {
            label = name;
            start_ts = start;
            duration = dur;
            parent = nullptr;
            total_children_duration = 0u;
        }
        
        HT_TimestampNs get_stop_ts() const
        {
            return start_ts + duration;
        }
    };

    const std::string callgrind_header = "# callgrind format";
    std::string _file_name;
    std::unordered_map<HT_ThreadId, TreeNode*> _calls;
    std::unordered_map<HT_ThreadId, std::vector<std::pair<TreeNode*, int>>> _root_calls;
    std::vector<std::pair<HT_ThreadId, TreeNode>> _events;

    void _add_event(HT_ThreadId thread_id, 
                    std::string label,
                    HT_TimestampNs start_ts,
                    HT_DurationNs duration);
    void _add_new_calltree(HT_ThreadId thread_id, 
                           std::string label,
                           HT_TimestampNs start_ts,
                           HT_DurationNs duration);
    void _print_function(std::ofstream& file, TreeNode* node, std::string label);
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
