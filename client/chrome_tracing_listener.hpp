#ifndef HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP
#define HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP

#include <hawktracer/parser/event.hpp>
#include "tracepoint_map.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class ChromeTraceListener
{
public:
    ChromeTraceListener(std::unique_ptr<TracepointMap> tracepoint_map);
    ~ChromeTraceListener();

    bool init(const std::string& file_name);
    void uninit();
    void process_event(const parser::Event& event);

private:
    std::ofstream file;
    std::unique_ptr<TracepointMap> _tracepoint_map;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP
