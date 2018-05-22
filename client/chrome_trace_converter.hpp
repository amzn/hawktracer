#ifndef HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "converter.hpp" 
#include "tracepoint_map.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class ChromeTraceConverter : public Converter
{
public:
    ~ChromeTraceConverter() override;

    bool init(const std::string& file_name) override;
    void uninit() override;
    void process_event(const parser::Event& event) override;
    bool stop() override;

private:
    std::ofstream _file;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
