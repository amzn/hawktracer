#include <hawktracer/listeners/tcp_listener.h>

#include <gtest/gtest.h>

TEST(TestTcpListener, DISABLED_ApplicationShouldNotCrashWhenCreatingListenerFails)
{
    // Arrange
    const int port = 8787;
    HT_ErrorCode err;
    HT_TCPListener* listener1 = ht_tcp_listener_create(port, 2048, nullptr);

    // Act
    HT_TCPListener* listener2 = ht_tcp_listener_create(port, 2048, &err);

    // Assert
    ht_tcp_listener_destroy(listener1);
    ASSERT_EQ(nullptr, listener2);
    ASSERT_NE(HT_ERR_OK, err);
}
