#include <gtest/gtest.h>
#include "hawktracer/client_utils/stream_factory.hpp"

#include <fstream>

using HawkTracer::ClientUtils::make_stream_from_string;

TEST(TestStreamFactory, ShouldNotFailIfInvalidPortSpecified)
{
    // Arrange
    // Act & Assert
    ASSERT_TRUE(make_stream_from_string("127.0.0.1:x"));
}

TEST(TestStreamFactory, ShouldNotFailIfPortSpecified)
{
    // Arrange
    // Act & Assert
    ASSERT_TRUE(make_stream_from_string("127.0.0.1:1234"));
}

TEST(TestStreamFactory, ShouldNotFailIfPortNotSpecified)
{
    // Arrange
    // Act & Assert
    ASSERT_TRUE(make_stream_from_string("127.0.0.1"));
}

TEST(TestStreamFactory, ShouldNotFailIfNotEnoughIPComponents)
{
    // Arrange
    // Act & Assert
    ASSERT_FALSE(make_stream_from_string("127.0.0:1234"));
}

TEST(TestStreamFactory, ShouldNotFailIfFileExists)
{
    // Arrange
    std::string file_name = "test_stream_factory.txt";
    std::ofstream f(file_name);
    f.close();

    // Act & Assert
    EXPECT_TRUE(make_stream_from_string(file_name));

    // Cleanup
    remove(file_name.c_str());
}
