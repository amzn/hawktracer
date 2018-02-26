#ifndef HAWKTRACER_PARSER_DEBUG_EVENT_LISTENER_HPP
#define HAWKTRACER_PARSER_DEBUG_EVENT_LISTENER_HPP

#include "parser/event.hpp"

namespace HawkTracer
{
namespace parser
{

class DebugEventListener
{
public:
    void operator()(const Event& event)
    {
        _process_event(event);
    }

    void print_statistics() const;

private:
    void _process_event(const Event& event);

    std::unordered_map<std::string, size_t> _event_stats;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_DEBUG_EVENT_LISTENER_HPP
