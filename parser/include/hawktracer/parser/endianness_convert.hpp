#ifndef HAWKTRACER_PARSER_ENDIANNESS_CONVERT_HPP
#define HAWKTRACER_PARSER_ENDIANNESS_CONVERT_HPP

#include <hawktracer/system_info.h>

namespace HawkTracer
{
namespace parser
{

#define HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(ENDIANNESS) \
    if (is_system_endianness(ENDIANNESS)) return x

inline bool is_system_endianness(HT_Endianness endianness)
{
    static HT_Endianness sys_endianness = ht_system_info_get_endianness();

    return sys_endianness == endianness;
}

inline int8_t convert_endianness_to_native(int8_t x, HT_Endianness)
{
    return x;
}

inline uint8_t convert_endianness_to_native(uint8_t x, HT_Endianness)
{
    return x;
}

inline int16_t convert_endianness_to_native(int16_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    return (uint16_t(x) << 8) | (uint16_t(x) >> 8);
}

inline uint16_t convert_endianness_to_native(uint16_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    return  (x << 8) | (x >> 8);
}

inline int32_t convert_endianness_to_native(int32_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    uint32_t tmp = uint32_t(x) << 16 | uint32_t(x) >> 16;
    return ((uint32_t(tmp) << 8) & 0xff00ff00) | ((uint32_t(tmp) >> 8) & 0x00ff00ff);
}
inline uint32_t convert_endianness_to_native(uint32_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    uint32_t tmp = x << 16 | x >> 16;
    return ((tmp << 8) & 0xff00ff00) | ((tmp >> 8) & 0x00ff00ff);
}

inline int64_t convert_endianness_to_native(int64_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    uint64_t tmp1 = uint64_t(x) << 32 | uint64_t(x) >> 32;
    uint64_t tmp2 = (tmp1 & 0x0000FFFF0000FFFFULL) << 16 | (tmp1 & 0xFFFF0000FFFF0000ULL) >> 16;
    return int64_t((tmp2 & 0x00FF00FF00FF00FFULL) << 8 | (tmp2 & 0xFF00FF00FF00FF00ULL) >> 8);
}

inline uint64_t convert_endianness_to_native(uint64_t x, HT_Endianness from_endianness)
{
    HAWKTRACER_PARSER_SYSTEM_ENDIANNESS(from_endianness);

    uint64_t tmp1 = x << 32 | x >> 32;
    uint64_t tmp2 = (tmp1 & 0x0000FFFF0000FFFFULL) << 16 | (tmp1 & 0xFFFF0000FFFF0000ULL) >> 16;
    return (tmp2 & 0x00FF00FF00FF00FFULL) << 8 | (tmp2 & 0xFF00FF00FF00FF00ULL) >> 8;
}

#undef HAWKTRACER_PARSER_SYSTEM_ENDIANNESS

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_ENDIANNESS_CONVERT_HPP
