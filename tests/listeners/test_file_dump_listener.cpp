#include <hawktracer/listeners/file_dump_listener.h>
#include <hawktracer/global_callstack_timeline.h>

#include <gtest/gtest.h>

static const char* test_file = "dump_listener_test_file_test_file";

TEST(TestFileDumpListener, InitShouldFailIfFileCanNotBeOpened)
{
    // Arrange
    HT_FileDumpListener listener;

    // Act & Assert
    ASSERT_FALSE(ht_file_dump_listener_init(&listener, "/non/existing/file"));
}

TEST(TestFileDumpListener, FlushingTimelineShouldAddEventToInternalBuffer)
{
    // Arrange
    HT_FileDumpListener listener;
    HT_Timeline* timeline = HT_TIMELINE(ht_global_callstack_timeline_get());

    ht_file_dump_listener_init(&listener, test_file);
    ht_timeline_register_listener(timeline, ht_file_dump_listener_callback, &listener);

    HT_DECL_EVENT(HT_Event, event);
    event.id = 1;
    event.timestamp = 1;
    ht_timeline_push_event(timeline, &event);

    // Act
    ht_timeline_flush(timeline);

    // Assert
    ht_timeline_unregister_all_listeners(timeline);
    ASSERT_LT(0u, listener.buffer_usage);

    ht_file_dump_listener_deinit(&listener);
}

TEST(TestFileDumpListener, ManyEventsShouldCauseDumpToFile)
{
    // Arrange
    HT_FileDumpListener listener;
    HT_Timeline* timeline = HT_TIMELINE(ht_global_callstack_timeline_get());

    remove(test_file);
    ht_file_dump_listener_init(&listener, test_file);
    ht_timeline_register_listener(timeline, ht_file_dump_listener_callback, &listener);

    // Act
    for (size_t i = 0; i < HT_FILE_DUMP_LISTENER_BUFFER_SIZE; i++)
    {
        HT_DECL_EVENT(HT_Event, event);
        event.id = 1;
        event.timestamp = 1;
        ht_timeline_push_event(timeline, &event);

        ht_timeline_flush(timeline);
    }

    // Assert
    ht_timeline_unregister_all_listeners(timeline);
    ht_file_dump_listener_deinit(&listener);

    FILE* fp = fopen(test_file, "rb");
    ASSERT_NE(nullptr, fp);
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fclose(fp);
    ASSERT_LT(HT_FILE_DUMP_LISTENER_BUFFER_SIZE, file_size);
}
