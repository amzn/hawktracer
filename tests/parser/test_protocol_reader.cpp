#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

TEST(TestProtocolReader, StartShouldFailIfStreamFailsToStart)
{
    // Arrange
    class TestFailingStream : public Stream
    {
        int read_byte() override { return 0; }
        bool read_data(char* /* buff */, size_t /* size */) override { return false; }
        bool start() override { return false; }
        bool is_continuous() override { return true; }
    };
    KlassRegister klass_register;
    ProtocolReader reader(&klass_register, make_unique<TestFailingStream>(), false);

    // Act & Assert
    ASSERT_FALSE(reader.start());
}
