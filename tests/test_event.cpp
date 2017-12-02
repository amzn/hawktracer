#include <hawktracer/events.h>

#include <gtest/gtest.h>

#define _BUFF_TO_TYPE(buffer, type) *(type*)(buffer)

TEST(TestEvent, CallstackIntEventSerialize)
{
    // Arrange
    HT_DECL_EVENT(HT_CallstackIntEvent, event)
    HT_Byte buffer[1024];

    HT_EVENT(&event)->timestamp = 9381;
    HT_EVENT(&event)->id = 132;
    HT_CALLSTACK_BASE_EVENT(&event)->duration = 332;
    HT_CALLSTACK_BASE_EVENT(&event)->thread_id = 8;
    event.label = 83123;

    // Act
    HT_EVENT_SERIALIZE(&event, buffer);

    // Assert
    ASSERT_EQ(_BUFF_TO_TYPE(buffer, HT_EventType), HT_EVENT_GET_CLASS(&event)->type);
    size_t offset = sizeof(HT_EventType);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_TimestampNs), HT_EVENT(&event)->timestamp);
    offset += sizeof(HT_TimestampNs);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_EventId), HT_EVENT(&event)->id);
    offset += sizeof(HT_EventId);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_DurationNs), HT_CALLSTACK_BASE_EVENT(&event)->duration);
    offset += sizeof(HT_DurationNs);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_ThreadId), HT_CALLSTACK_BASE_EVENT(&event)->thread_id);
    offset += sizeof(HT_ThreadId);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_CallstackEventLabel), event.label);
}

TEST(TestEvent, CallstackStringEventSerialize)
{
    // Arrange
    HT_DECL_EVENT(HT_CallstackStringEvent, event)
    HT_Byte buffer[1024];

    HT_EVENT(&event)->timestamp = 9381;
    HT_EVENT(&event)->id = 132;
    HT_CALLSTACK_BASE_EVENT(&event)->duration = 332;
    HT_CALLSTACK_BASE_EVENT(&event)->thread_id = 8;
    strncpy(event.label, "hello_world", 11);

    // Act
    HT_EVENT_SERIALIZE(&event, buffer);

    // Assert
    ASSERT_EQ(_BUFF_TO_TYPE(buffer, HT_EventType), HT_EVENT_GET_CLASS(&event)->type);
    size_t offset = sizeof(HT_EventType);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_TimestampNs), HT_EVENT(&event)->timestamp);
    offset += sizeof(HT_TimestampNs);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_EventId), HT_EVENT(&event)->id);
    offset += sizeof(HT_EventId);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_DurationNs), HT_CALLSTACK_BASE_EVENT(&event)->duration);
    offset += sizeof(HT_DurationNs);
    ASSERT_EQ(_BUFF_TO_TYPE(buffer + offset, HT_ThreadId), HT_CALLSTACK_BASE_EVENT(&event)->thread_id);
    offset += sizeof(HT_ThreadId);
    ASSERT_STREQ((char*)buffer + offset, event.label);
}
