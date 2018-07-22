#include <gtest/gtest.h>

#include "hawktracer/client_utils/command_line_parser.hpp"

#include <vector>

using HawkTracer::ClientUtils::CommandLineParser;

bool parse(CommandLineParser& parser, std::vector<std::string> params)
{
    params.insert(params.begin(), "app");
    char** c_params = new char*[params.size()];
    for (size_t i = 0; i < params.size(); i++)
    {
        c_params[i] = const_cast<char*>(params[i].c_str());
    }
    bool ret = parser.parse(params.size(), c_params);
    delete [] c_params;
    return ret;
}

TEST(TestCommandLineParser, ParseShouldFailIfInvalidPrefixUsed)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt", CommandLineParser::OptionInfo(false, false, ""));

    // Act & Assert
    ASSERT_FALSE(parse(parser, {"-test"}));
}

TEST(TestCommandLineParser, ParseShouldFailIfOptionNotRegistered)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt", CommandLineParser::OptionInfo(false, false, ""));

    // Act & Assert
    ASSERT_FALSE(parse(parser, {"--opt2"}));
}

TEST(TestCommandLineParser, ParseShouldNotFailIfOptionIsFlagAndDoesNotHaveValue)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt", CommandLineParser::OptionInfo(true, false, ""));
    parser.register_option("other", CommandLineParser::OptionInfo(false, false, ""));

    // Act & Assert
    ASSERT_TRUE(parse(parser, {"--opt", "--other", "value"}));
}

TEST(TestCommandLineParser, ParseShouldFailIfOptionIsFlagAndHasValue)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt", CommandLineParser::OptionInfo(true, false, ""));

    // Act & Assert
    ASSERT_FALSE(parse(parser, {"--opt", "value"}));
}

TEST(TestCommandLineParser, ParseShouldFailIfMandatoryOptionNotSpecified)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));
    parser.register_option("opt2", CommandLineParser::OptionInfo(false, true, ""));

    // Act & Assert
    ASSERT_FALSE(parse(parser, {"--opt1", "value"}));
}

TEST(TestCommandLineParser, ParseShouldNotFailIfOptionalOptionNotSpecified)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));
    parser.register_option("opt2", CommandLineParser::OptionInfo(false, false, ""));

    // Act & Assert
    ASSERT_TRUE(parse(parser, {"--opt1", "value"}));
}

TEST(TestCommandLineParser, ParseShouldFailIfOptionMissesValue)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));

    // Act & Assert
    ASSERT_FALSE(parse(parser, {"--opt1"}));
}

TEST(TestCommandLineParser, GetValueShouldReturnDefaultValueIfValueNotSpecified)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));
    ASSERT_TRUE(parse(parser, {}));

    // Act & Assert
    ASSERT_STREQ("default", parser.get_value("opt1", "default").c_str());
}

TEST(TestCommandLineParser, GetValueShouldReturnUserSpecifiedValueIfValue)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));
    ASSERT_TRUE(parse(parser, {"--opt1", "value"}));

    // Act & Assert
    ASSERT_STREQ("value", parser.get_value("opt1", "default").c_str());
}

TEST(TestCommandLineParser, HasValueShouldReturnTrueIfValueSpecified)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(true, false, ""));
    ASSERT_TRUE(parse(parser, {"--opt1"}));

    // Act & Assert
    ASSERT_TRUE(parser.has_value("opt1"));
}

TEST(TestCommandLineParser, HasValueShouldReturnFalseIfValueNotSpecified)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(true, false, ""));
    ASSERT_TRUE(parse(parser, {}));

    // Act & Assert
    ASSERT_FALSE(parser.has_value("opt1"));
}

TEST(TestCommandLineParser, RegisterMandatoryFlagShouldFail)
{
    // Arrange
    CommandLineParser parser("--", "test");

    // Act
    parser.register_option("opt1", CommandLineParser::OptionInfo(true, true, ""));

    // Assert
    ASSERT_FALSE(parse(parser, {"--opt1", "value"})); // should fail as "opt1" is not registered
}

TEST(TestCommandLineParser, PrintHelpShouldContainAllOptions)
{
    // Arrange
    CommandLineParser parser("--", "test");
    parser.register_option("opt1", CommandLineParser::OptionInfo(false, false, ""));
    parser.register_option("opt2", CommandLineParser::OptionInfo(false, true, ""));

    std::stringstream ss;

    // Act
    parser.print_help(ss);

    // Assert
    std::string help_str = ss.str();
    ASSERT_NE(std::string::npos, help_str.find("--opt1"));
    ASSERT_NE(std::string::npos, help_str.find("--opt2"));
}
