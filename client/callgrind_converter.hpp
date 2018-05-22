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
        HT_TimestampNs duration;
        std::vector<std::pair<TreeNode*, int>> children;
        TreeNode* father;
        HT_TimestampNs total_children_duration;

        TreeNode(std::string name, HT_TimestampNs start, HT_TimestampNs dur)
        {
            label = name;
            start_ts = start;
            duration = dur;
            father = nullptr;
            total_children_duration = 0u;
        }
        
        HT_TimestampNs get_stop_ts()
        {
            return start_ts + duration;
        }
    };

    std::string _file_name;
    std::unordered_map<uint32_t, TreeNode*> _calls;
    std::unordered_map<uint32_t, std::vector<std::pair<TreeNode*, int>>> _root_calls;
    std::vector<std::pair<uint32_t, TreeNode>> _events;

    void _add_event(uint32_t thread_id, 
                    std::string label,
                    HT_TimestampNs start_ts,
                    HT_TimestampNs duration);
    void _add_new_calltree(uint32_t thread_id, 
                           std::string label,
                           HT_TimestampNs start_ts,
                           HT_TimestampNs duration);
    void _print_function(std::ofstream& file, TreeNode* node, std::string label);
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
