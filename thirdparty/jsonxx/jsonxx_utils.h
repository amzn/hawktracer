#include "jsonxx.h"

template<typename T, typename json_T>
static void try_get_value(const jsonxx::Object& json_obj, const std::string& field_name, T& field)
{
    if (json_obj.has<json_T>(field_name))
    {
        field = json_obj.get<json_T>(field_name);
    }
}
