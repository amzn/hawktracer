#ifndef HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP
#define HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP

#include "parser/event.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class ChromeTraceListener
{
public:
    ~ChromeTraceListener();

    bool init(const std::string& file_name);
    void uninit();
    void process_event(const parser::Event& event);

private:
    std::ofstream file;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CHROME_TRACING_LISTENER_HPP
