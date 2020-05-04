#ifndef HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP

#include <hawktracer/client_utils/converter.hpp>
#include <hawktracer/parser/event.hpp>

#include <fstream>

namespace HawkTracer
{
namespace client
{

class ChromeTraceConverter : public ClientUtils::Converter
{
public:
    ~ChromeTraceConverter() override;

    bool init(const std::string& file_name) override;
    void process_event(const parser::Event& event) override;
    void stop() override;

private:
    static std::string _get_args(const parser::Event& event);
    static std::string _get_json_value(const parser::Event::Value& value);

    std::ofstream _file;
    bool _first_event_saved = false;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CHROME_TRACE_CONVERTER_HPP
