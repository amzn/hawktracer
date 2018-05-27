#include <hawktracer/parser/endianness_convert.hpp>

#include <gtest/gtest.h>

using HawkTracer::parser::convert_endianness_to_native;

class TestEndiannessConvert : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        _source_endianness = ht_system_info_get_endianness() == HT_ENDIANNESS_BIG ?
                    HT_ENDIANNESS_LITTLE : HT_ENDIANNESS_BIG;
    }

    static HT_Endianness _source_endianness;
};

HT_Endianness TestEndiannessConvert::_source_endianness;

#define DEFINE_CONVERT_TESTS(C_TYPE, EXPECTED_VALUE, INPUT_VALUE) \
    TEST_F(TestEndiannessConvert, DifferentEndiannessShouldChangeByteOrder_##C_TYPE) \
    { \
        ASSERT_EQ(static_cast<C_TYPE>(EXPECTED_VALUE), convert_endianness_to_native(static_cast<C_TYPE>(INPUT_VALUE), _source_endianness)); \
    } \
    TEST_F(TestEndiannessConvert, TheSameEndiannessShouldNotChangeByteOrder_##C_TYPE) \
    { \
        ASSERT_EQ(static_cast<C_TYPE>(INPUT_VALUE), convert_endianness_to_native(static_cast<C_TYPE>(INPUT_VALUE), ht_system_info_get_endianness())); \
    }

DEFINE_CONVERT_TESTS(uint64_t, 0x0123456789ABCDAAu, 0xAACDAB8967452301u)
DEFINE_CONVERT_TESTS(int64_t, 0x01CDAB8967452301, 0x0123456789ABCD01)
DEFINE_CONVERT_TESTS(uint32_t, 0x01234567u, 0x67452301u)
DEFINE_CONVERT_TESTS(int32_t, 0x01234501, 0x01452301)
DEFINE_CONVERT_TESTS(uint16_t, 0x0123u, 0x2301u)
DEFINE_CONVERT_TESTS(int16_t, 0x0123, 0x2301)
DEFINE_CONVERT_TESTS(uint8_t, 0xABu, 0xABu)
DEFINE_CONVERT_TESTS(int8_t, 0x56, 0x56)

