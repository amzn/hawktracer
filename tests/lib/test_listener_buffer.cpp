#include <hawktracer/listener_buffer.h>
#include <hawktracer/events.h>

#include <gtest/gtest.h>

TEST(TestListenerBuffer, ShouldCallCallbackForSerializedEvent)
{
    // Arrange
    HT_ListenerBuffer buffer;
    HT_Byte buff[10] = {0};
    ht_listener_buffer_init(&buffer, 4);
    int num_calls = 0;

    // Act
    ht_listener_buffer_process_serialized_events(
                &buffer, buff, 10, [] (void* user_data) {++*((int*)user_data);}, &num_calls);

    // Assert
    ASSERT_EQ(2, num_calls);
    ht_listener_buffer_deinit(&buffer);
}

TEST(TestListenerBuffer, ShouldCallCallbackForUnserializedEvent)
{
    // Arrange
    HT_ListenerBuffer buffer;
    HT_Byte buff[sizeof(HT_Event) * 3] = {0};
    HT_DECL_EVENT(HT_Event, ev);
    memcpy(buff, (char*)&ev, sizeof(ev));
    memcpy(buff + sizeof(ev), (char*)&ev, sizeof(ev));
    memcpy(buff + 2 * sizeof(ev), (char*)&ev, sizeof(ev));

    ht_listener_buffer_init(&buffer, sizeof(HT_Event));
    int num_calls = 0;

    // Act
    ht_listener_buffer_process_unserialized_events(
                &buffer, buff, 3 * sizeof(HT_Event), [] (void* user_data) {++*((int*)user_data);}, &num_calls);

    // Assert
    ASSERT_EQ(2, num_calls);
    ht_listener_buffer_deinit(&buffer);
}
