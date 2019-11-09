#include <internal/command_line_parser.h>
#include <internal/global_timeline.h>

#include "test_common.h"

class TestCommandLineParserLib : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _buff_size = ht_global_timeline_get_buffer_size();
    }

    void TearDown() override
    {
        ht_global_timeline_set_buffer_size(_buff_size);
    }

    size_t _buff_size;
};

TEST_F(TestCommandLineParserLib, SettingBufferSizeShouldPassForValidInput)
{
    // Arrange
    const char* args[] = {"app", "--ht-global-timeline-buffer-size", "76"};

    // Act
    ht_command_line_parse_args(3, (char**)args);

    // Assert
    ASSERT_EQ(76u, ht_global_timeline_get_buffer_size());

    // Cleanup
    ht_global_timeline_set_buffer_size(_buff_size);
}

TEST_F(TestCommandLineParserLib, SettingBufferSizeShouldFailForOutOfRangeValue)
{
    // Arrange
    const char* args[] = {"app", "--ht-global-timeline-buffer-size", "9999999999999999999999999999"};

    // Act
    ht_command_line_parse_args(3, (char**)args);

    // Assert
    ASSERT_EQ(_buff_size, ht_global_timeline_get_buffer_size());
}

TEST_F(TestCommandLineParserLib, SettingBufferSizeShouldFailForInvalidString)
{
    // Arrange
    const char* args[] = {"app", "--ht-global-timeline-buffer-size", "test"};

    // Act
    ht_command_line_parse_args(3, (char**)args);

    // Assert
    ASSERT_EQ(_buff_size, ht_global_timeline_get_buffer_size());
}

TEST_F(TestCommandLineParserLib, SettingBufferSizeShouldFailIfValueIsMissing)
{
    // Arrange
    const char* args[] = {"app", "--ht-global-timeline-buffer-size"};

    // Act
    ht_command_line_parse_args(2, (char**)args);

    // Assert
    ASSERT_EQ(_buff_size, ht_global_timeline_get_buffer_size());
}

TEST_F(TestCommandLineParserLib, PassingInvalidArgumentShouldSkipTheArgument)
{
    // Arrange
    const char* args[] = {"app", "--ht-non-existing-parameter"};

    // Act
    ht_command_line_parse_args(2, (char**)args);
}

