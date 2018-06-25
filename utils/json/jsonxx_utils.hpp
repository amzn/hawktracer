#include <thirdparty/jsonxx/jsonxx.h>

namespace HawkTracer
{
namespace utils
{

template<typename T, typename json_T>
static bool get_value(const jsonxx::Object& json_obj, const std::string& field_name, T& field)
{
    if (json_obj.has<json_T>(field_name))
    {
        field = json_obj.get<json_T>(field_name);
        return true;
    }
    return false;
}

} // namespace utils
} // namespace HawkTracer
