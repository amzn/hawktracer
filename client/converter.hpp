#ifndef HAWKTRACER_CLIENT_CONVERTER_HPP
#define HAWKTRACER_CLIENT_CONVERTER_HPP

#include <hawktracer/parser/event.hpp>
#include "label_mapping.hpp"

#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

class Converter
{
public:
    Converter(std::vector<std::string> map_files);

    virtual ~Converter() {}
    virtual bool init(const std::string& file_name) = 0;
    virtual void process_event(const parser::Event& event) = 0;
    virtual void stop() = 0;

protected:
    std::unique_ptr<LabelMapping> _label_mapping;
};

} // namespace client
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_CONVERTER_HPP
