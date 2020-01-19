#include <hawktracer/listeners/file_dump_listener.h>
#include <hawktracer/global_timeline.h>

#include <gtest/gtest.h>

static const char* test_file = "dump_listener_test_file_test_file";

static HT_Timeline* create_timeline()
{
    return ht_timeline_create(1024, HT_FALSE, HT_TRUE, nullptr, nullptr);
}

class TestFileDumpListener : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        _registry_klass_bytes = ht_timeline_listener_push_metadata(
                    [](TEventPtr, size_t, HT_Boolean, void*){}, nullptr, HT_TRUE);
    }

    static size_t _registry_klass_bytes;
};

size_t TestFileDumpListener::_registry_klass_bytes;

TEST_F(TestFileDumpListener, InitShouldFailIfFileCanNotBeOpened)
{
    // Arrange
    HT_ErrorCode error;

    // Act
    HT_FileDumpListener* listener = ht_file_dump_listener_create("/non/existing/file", 4096u, &error);

    // Assert
    ASSERT_EQ(nullptr, listener);
    ASSERT_EQ(HT_ERR_CANT_OPEN_FILE, error);
}

TEST_F(TestFileDumpListener, EventShouldBeCorrectlyStoredInAFile)
{
    // Arrange
    HT_Timeline* timeline = create_timeline();

    ht_file_dump_listener_register(timeline, test_file, 4096u, nullptr);

    HT_DECL_EVENT(HT_Event, event);
    event.id = 32;
    event.timestamp = 9983;

    // Act
    ht_timeline_push_event(timeline, &event);
    ht_timeline_destroy(timeline);

    // Assert
    FILE* fp = fopen(test_file, "rb");
    ASSERT_NE(nullptr, fp);
    fseek(fp, _registry_klass_bytes, SEEK_SET);
    char buff[64];
    EXPECT_EQ(event.klass->get_size(&event), fread(buff, sizeof(char), 64, fp));
    size_t offset = 0;
#define ASSERT_FROM_BUFFER(event_value) \
    EXPECT_EQ(event_value, *(decltype(&event_value)(buff + offset))); offset += sizeof(event_value)
    ASSERT_FROM_BUFFER(event.klass->klass_id);
    ASSERT_FROM_BUFFER(event.timestamp);
    ASSERT_FROM_BUFFER(event.id);
#undef ASSERT_FROM_BUFFER

    fclose(fp);
}

TEST_F(TestFileDumpListener, ManyEventsShouldCauseDumpToFile)
{
    // Arrange
    HT_Timeline* timeline = create_timeline();
    const size_t buffer_size = 4096u;

    remove(test_file);
    ht_file_dump_listener_register(timeline, test_file, buffer_size, nullptr);

    // Act
    for (size_t i = 0; i < buffer_size; i++)
    {
        HT_DECL_EVENT(HT_Event, event);
        event.id = 1;
        event.timestamp = 1;
        ht_timeline_push_event(timeline, &event);

        ht_timeline_flush(timeline);
    }

    // Assert
    ht_timeline_destroy(timeline);
    FILE* fp = fopen(test_file, "rb");
    ASSERT_NE(nullptr, fp);
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fclose(fp);
    ASSERT_LT(buffer_size, file_size);
}

TEST_F(TestFileDumpListener, NonSerializedTimeline)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_FALSE, NULL, NULL);

    ht_file_dump_listener_register(timeline, test_file, 4096u, nullptr);

    HT_DECL_EVENT(HT_Event, event);
    event.id = 32;
    event.timestamp = 9983;

    // Act
    ht_timeline_push_event(timeline, &event);
    ht_timeline_destroy(timeline);

    // Assert
    FILE* fp = fopen(test_file, "rb");
    ASSERT_NE(nullptr, fp);
    fseek(fp, _registry_klass_bytes, SEEK_SET);
    char buff[64];
    EXPECT_EQ(event.klass->get_size(&event), fread(buff, sizeof(char), 64, fp));
    size_t offset = 0;
#define ASSERT_FROM_BUFFER(event_value) \
    EXPECT_EQ(event_value, *(decltype(&event_value)(buff + offset))); offset += sizeof(event_value)
    ASSERT_FROM_BUFFER(event.klass->klass_id);
    ASSERT_FROM_BUFFER(event.timestamp);
    ASSERT_FROM_BUFFER(event.id);
#undef ASSERT_FROM_BUFFER

    fclose(fp);
}

#ifdef __linux__

#include <sys/stat.h>

TEST_F(TestFileDumpListener, ForceFlushListenerShouldUpdateFileImmediately)
{
    // Arrange
    HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, NULL);
    HT_FileDumpListener* listener = ht_file_dump_listener_register(timeline, test_file, 4096u, nullptr);

    HT_DECL_EVENT(HT_Event, event);
    event.id = 32;
    event.timestamp = 9983;

    struct stat st;
    size_t size_before, size_after;

    // Act
    ht_timeline_flush(timeline);
    ht_file_dump_listener_flush(listener, HT_TRUE);

    stat(test_file, &st);
    size_before = st.st_size;

    ht_timeline_push_event(timeline, &event);
    ht_timeline_flush(timeline);
    ht_file_dump_listener_flush(listener, HT_TRUE);

    stat(test_file, &st);
    size_after = st.st_size;

    // Assert
    ASSERT_EQ(HT_EVENT_GET_KLASS(&event)->get_size(&event), size_after - size_before);

    ht_timeline_destroy(timeline);
}

#endif /* __linux__ */

