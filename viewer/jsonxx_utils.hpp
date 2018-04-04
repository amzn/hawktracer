#ifndef HAWKTRACER_VIEWER_JSONXX_UTILS_HPP
#define HAWKTRACER_VIEWER_JSONXX_UTILS_HPP

#include "jsonxx.h"

namespace HawkTracer
{
namespace viewer
{

template<typename T1>
inline bool check_required_fields(const jsonxx::Object& obj, std::vector<const char*> fields, size_t pos = 0)
{
    return obj.has<T1>(fields[pos]);
}

template<typename T1, typename T2, typename ...T>
inline bool check_required_fields(const jsonxx::Object& obj, std::vector<const char*> fields, size_t pos = 0)
{
    return obj.has<T1>(fields[pos]) && check_required_fields<T2, T...>(obj, fields, pos+1);
}

template<typename T>
inline void append_to_json_object(jsonxx::Object& obj, std::string key, T&& value)
{
    obj << std::move(key) << value;
}

template<typename T>
inline void append_to_json_object(jsonxx::Object& obj, const char* key, T&& value)
{
    append_to_json_object(obj, std::string(key), std::forward<T>(value));
}

template<typename K, typename V, typename ...T>
inline void append_to_json_object(jsonxx::Object& obj, K&& key, V&& value, T&& ...values)
{
    append_to_json_object(obj, std::move(key), value);
    append_to_json_object(obj, std::forward<T>(values)...);
}

template<typename ...Args>
inline jsonxx::Object make_json_object(Args&&... args)
{
    jsonxx::Object obj;

    append_to_json_object(obj, std::forward<Args>(args)...);

    return obj;
}

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_JSONXX_UTILS_HPP
