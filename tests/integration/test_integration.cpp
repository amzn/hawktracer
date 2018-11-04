#include <gtest/gtest.h>

#include <hawktracer/timeline.h>

#include <hawktracer/parser/event.hpp>
#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <hawktracer/core_events.h>

#include <hawktracer/event_macros_impl.h>

HT_DECLARE_EVENT_KLASS(IntegrationTestEvent, HT_Event,
                       (INTEGER, uint8_t, uint8_t_field),
                       (INTEGER, uint16_t, uint16_t_field),
                       (INTEGER, uint32_t, uint32_t_field),
                       (INTEGER, uint64_t, uint64_t_field),
                       (INTEGER, int8_t, int8_t_field),
                       (INTEGER, int16_t, int16_t_field),
                       (INTEGER, int32_t, int32_t_field),
                       (INTEGER, int64_t, int64_t_field),
                       (STRING, const char*, string_field))

HT_DECLARE_EVENT_KLASS(PointerTestEvent, HT_Event,
                       (POINTER, void*, void_ptr_field))

HT_DECLARE_EVENT_KLASS(IntegrationTestExtendedEvent, IntegrationTestEvent,
                       (INTEGER, int32_t, extended_field))

using namespace HawkTracer::parser;

class MemoryStream : public Stream
{
public:
    explicit MemoryStream(std::vector<HT_Byte> data) :
        _data(std::move(data)) {}

    int read_byte() override
    {
        return _data.at(pos++);
    }

    bool read_data(char *buff, size_t size) override
    {
        if (pos == _data.size())
        {
            return false;
        }

        auto len = std::min(size, _data.size() - pos);
        memcpy(buff, _data.data() + pos, len);
        pos += len;
        return true;
    }

    bool is_continuous() override
    {
        return false;
    }

private:
    std::vector<HT_Byte> _data;
    size_t pos = 0;
};

class TestIntegration : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        HT_REGISTER_EVENT_KLASS(IntegrationTestEvent);
        HT_REGISTER_EVENT_KLASS(IntegrationTestExtendedEvent);
        HT_REGISTER_EVENT_KLASS(PointerTestEvent);
    }

protected:
    static std::vector<HT_Byte> _generate_data(std::function<void(HT_Timeline* timeline)> push_event_callback)
    {
        std::vector<HT_Byte> data;
        HT_ErrorCode err = HT_ERR_OK;
        HT_Timeline* timeline = ht_timeline_create(1024, HT_FALSE, HT_TRUE, NULL, &err);

        auto event_callback =[](TEventPtr events, size_t event_count, HT_Boolean serialized, void* user_data)
        {
            auto data = static_cast<std::vector<HT_Byte>*>(user_data);
            assert(serialized);

            data->insert(data->end(), events, events + event_count);
        };

        EXPECT_EQ(HT_ERR_OK, err);

        ht_timeline_register_listener(timeline, event_callback, &data);
        ht_timeline_listener_push_metadata(event_callback, &data, HT_TRUE);
        push_event_callback(timeline);

        ht_timeline_destroy(timeline);

        return data;
    }
};

TEST_F(TestIntegration, HandlingExtendedEventShouldNotFail)
{
    // Arrange
    KlassRegister registry;

    auto data = _generate_data([](HT_Timeline* timeline) {
            HT_TIMELINE_PUSH_EVENT(timeline, IntegrationTestExtendedEvent,
                                   1, 2, 3, 4, // UINT
                                   -1, -2, -3, -4, // INT
                                   "test", // STRING
                                   32 // extended
                                   );
    });

    ProtocolReader reader(&registry, HawkTracer::parser::make_unique<MemoryStream>(std::move(data)), true);
    Event expected_event(nullptr);

    reader.register_events_listener([&expected_event] (const Event& event) {
        if (event.get_klass()->get_id() == HT_EVENT_KLASS_GET(IntegrationTestExtendedEvent)->klass_id)
        {
            expected_event = event;
        }
    });

    // Act
    reader.start();
    reader.wait_for_complete();

    // Assert
    ASSERT_EQ(1u, expected_event.get_value<uint8_t>("uint8_t_field"));
    ASSERT_EQ(2u, expected_event.get_value<uint16_t>("uint16_t_field"));
    ASSERT_EQ(3u, expected_event.get_value<uint32_t>("uint32_t_field"));
    ASSERT_EQ(4u, expected_event.get_value<uint64_t>("uint64_t_field"));

    ASSERT_EQ(-1, expected_event.get_value<int8_t>("int8_t_field"));
    ASSERT_EQ(-2, expected_event.get_value<int16_t>("int16_t_field"));
    ASSERT_EQ(-3, expected_event.get_value<int32_t>("int32_t_field"));
    ASSERT_EQ(-4, expected_event.get_value<int64_t>("int64_t_field"));

    ASSERT_STREQ("test", expected_event.get_value<char*>("string_field"));

    ASSERT_EQ(32, expected_event.get_value<int32_t>("extended_field"));
}

TEST_F(TestIntegration, HandlePointerEventShouldFailAsItIsNotSupportedYet)
{
    // Arrange
    KlassRegister registry;

    auto data = _generate_data([](HT_Timeline* timeline) {
            HT_TIMELINE_PUSH_EVENT(timeline, PointerTestEvent, (void*)0x1234);
    });
    bool was_event = false;

    ProtocolReader reader(&registry, HawkTracer::parser::make_unique<MemoryStream>(std::move(data)), true);
    reader.register_events_listener([&was_event] (const Event& event) {
        if (event.get_klass()->get_id() == HT_EVENT_KLASS_GET(PointerTestEvent)->klass_id)
        {
            was_event = true;
        }
    });

    // Act
    reader.start();
    reader.wait_for_complete();

    // Assert
    ASSERT_FALSE(was_event);
}

TEST_F(TestIntegration, HandleBaseEventShouldNotFail)
{
    // Arrange
    KlassRegister registry;

    auto data = _generate_data([](HT_Timeline* timeline) {
            HT_Event event = {
                HT_EVENT_KLASS_GET(HT_Event),
                ht_monotonic_clock_get_timestamp(),
                ht_event_id_provider_next(ht_timeline_get_id_provider(timeline))
            };
            ht_timeline_push_event(timeline, &event);
    });
    bool was_event = false;

    ProtocolReader reader(&registry, HawkTracer::parser::make_unique<MemoryStream>(std::move(data)), true);
    reader.register_events_listener([&was_event] (const Event& event) {
        if (event.get_klass()->get_id() == HT_EVENT_KLASS_GET(HT_Event)->klass_id)
        {
            was_event = true;
        }
    });

    // Act
    reader.start();
    reader.wait_for_complete();

    // Assert
    ASSERT_TRUE(was_event);
}
