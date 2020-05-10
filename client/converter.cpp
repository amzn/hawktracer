#include "converter.hpp"

#include <hawktracer/parser/klass_register.hpp>
#include <hawktracer/parser/make_unique.hpp>
#include <fstream>
#include <stack>

namespace HawkTracer
{
namespace client
{

Converter::Converter(std::vector<std::string> map_files)
{
    auto map = parser::make_unique<TracepointMap>();
    map->load_maps(map_files);
    _label_mapping = parser::make_unique<LabelMapping>(std::move(map));
}

} // namespace client
} // namespace HawkTracer
