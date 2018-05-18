#ifndef HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "iconverter.hpp" 
#include "tracepoint_map.hpp"

#include <fstream>

namespace HawkTracer
{
namespace client
{

class ChromeTraceConverter : public IConverter
{
public:
    ChromeTraceConverter(std::shared_ptr<TracepointMap> tracepoint_map);
    ~ChromeTraceConverter();

    bool init(const std::string& file_name) override;
    void uninit() override;
    void process_event(const parser::Event& event) override;

private:
    const std::string _mapping_klass_name; 
    HT_EventKlassId _mapping_klass_id = 0;
    std::ofstream file;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
