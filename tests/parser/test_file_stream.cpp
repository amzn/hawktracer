#include <hawktracer/parser/file_stream.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

class TestFileStream : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        _data = {11u, 22u, 33u};
        _data_created_correctly = _generate_data();
    }

    static void TearDownTestCase()
    {
        remove(_file_name);
    }

    static bool _generate_data()
    {
        FILE* f = fopen(_file_name, "wb");
        if (!f)
        {
            return false;
        }

        bool ok = fwrite(_data.data(), sizeof(uint8_t), _data.size(), f) == _data.size();
        fclose(f);

        return ok;
    }

    static bool _data_created_correctly;
    static const char* _file_name;
    static std::vector<uint8_t> _data;
};

const char* TestFileStream::_file_name = "test_file_stream_test_file";
std::vector<uint8_t> TestFileStream::_data;
bool TestFileStream::_data_created_correctly = false;

// This test only checks if test file was created correctly
// This test is helpful when other tests are failing, so
// we might know why it happens.
TEST_F(TestFileStream, CheckIfTestDataWasCreatedCorrectly)
{
    ASSERT_TRUE(_data_created_correctly);
}

TEST_F(TestFileStream, StartShouldFailIfFileDoesNotExist)
{
    // Arrange
    FileStream stream("non-existing-file");

    // Act & Assert
    ASSERT_FALSE(stream.start());
}

TEST_F(TestFileStream, ReadByteShouldReturnCorrectValue)
{
    // Arrange
    FileStream stream(_file_name);
    stream.start();

    // Act
    int b1 = stream.read_byte();
    int b2 = stream.read_byte();

    // Assert
    ASSERT_EQ((int)_data[0], b1);
    ASSERT_EQ((int)_data[1], b2);
}

TEST_F(TestFileStream, ReadByteShouldReturnNegativeValueIfReachesEOF)
{
    // Arrange
    FileStream stream(_file_name);
    stream.start();

    // move pointer to end of file
    stream.read_byte();
    stream.read_byte();
    stream.read_byte();

    // Act
    int b = stream.read_byte();

    // Assert
    ASSERT_LT(b, 0);
}

TEST_F(TestFileStream, ReadDataShouldReturnDataFromCurrentPointer)
{
    // Arrange
    FileStream stream(_file_name);
    stream.start();
    char buf[2];

    stream.read_byte(); // move pointer

    // Act
    bool res = stream.read_data(buf, 2);

    // Assert
    ASSERT_TRUE(res);
    ASSERT_EQ(_data[1], static_cast<uint8_t>(buf[0]));
    ASSERT_EQ(_data[2], static_cast<uint8_t>(buf[1]));
}

TEST_F(TestFileStream, ReadDataShouldFailIfRequestTooMuchData)
{
    // Arrange
    FileStream stream(_file_name);
    stream.start();
    char buf[4];

    stream.read_byte(); // move pointer

    // Act
    bool res = stream.read_data(buf, 4);

    // Assert
    ASSERT_FALSE(res);
}
