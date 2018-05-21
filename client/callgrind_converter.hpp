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

private:
    std::ofstream _file;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CALLGRIND_CONVERTER_HPP
