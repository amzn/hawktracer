#ifndef HAWKTRACER_PARSER_MAKE_UNIQUE_HPP
#define HAWKTRACER_PARSER_MAKE_UNIQUE_HPP

#include <memory>

namespace HawkTracer
{
namespace parser
{

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_MAKE_UNIQUE_HPP
