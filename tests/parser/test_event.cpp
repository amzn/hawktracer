#include <hawktracer/parser/event.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

class TestParserEvent : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _klass = std::make_shared<EventKlass>("klass_name", 1);
        _klass_field1 = std::make_shared<EventKlassField>("field1", "field_type", FieldTypeId::INT8);
        _klass_field2 = std::make_shared<EventKlassField>("field2", "field_type", FieldTypeId::STRING);
        _klass_field3 = std::make_shared<EventKlassField>("field3", "field_type", FieldTypeId::UINT64);
    }

    std::shared_ptr<EventKlass> _klass;
    std::shared_ptr<EventKlassField> _klass_field1;
    std::shared_ptr<EventKlassField> _klass_field2;
    std::shared_ptr<EventKlassField> _klass_field3;
};

TEST_F(TestParserEvent, ValidateSetValueMethod)
{
   // Arrange
    Event event(_klass);
    FieldType value{};
    value.f_INT8 = 5;

    // Act
    event.set_value(_klass_field1.get(), value);

    // Assert
    ASSERT_EQ(value.f_INT8, event.get_value<int8_t>(_klass_field1->get_name()));
}

TEST_F(TestParserEvent, SetTimestampValueShouldUpdateTimestampField)
{
   // Arrange
    auto timestamp_field = std::make_shared<EventKlassField>("timestamp", "field_type", FieldTypeId::UINT64);
    Event event(_klass);
    FieldType value{};
    value.f_UINT64 = 987;

    // Act
    event.set_value(timestamp_field.get(), value);

    // Assert
    ASSERT_EQ(value.f_UINT64, event.get_timestamp());
}

TEST_F(TestParserEvent, MergeEventsShouldCopyAllValues)
{
    // Arrange
    Event event1(_klass);
    Event event2(_klass);
    const char* test_string = "aaa";
    int8_t value1 = 12;
    char* value2 = (char*)malloc(4); strncpy(value2, test_string, 4);
    uint64_t value3 = 15;

    event1.set_value(_klass_field1.get(), value1);
    event2.set_value(_klass_field2.get(), value2);
    event2.set_value(_klass_field3.get(), value3);

    // Act
    event1.merge(std::move(event2));

    // Assert
    ASSERT_EQ(3u, event1.get_values().size());
    ASSERT_EQ(value1, event1.get_value<int8_t>(_klass_field1->get_name()));
    ASSERT_STREQ(test_string, event1.get_value<char*>(_klass_field2->get_name()));
    ASSERT_EQ(value3, event1.get_value<uint64_t>(_klass_field3->get_name()));
}

TEST_F(TestParserEvent, CopyEventShouldCopyAllValues)
{
    // Arrange
    auto base_klass = std::make_shared<EventKlass>("base_klass", 1);
    auto base_klass_field1 = std::make_shared<EventKlassField>("base_field1", "field_type", FieldTypeId::INT8);
    auto base_klass_field2 = std::make_shared<EventKlassField>("base_field2", "field_type", FieldTypeId::STRING);
    auto base_klass_field3 = std::make_shared<EventKlassField>("base_field3", "klass_name", FieldTypeId::STRUCT);

    Event base_event(base_klass);
    const char* test_string = "abc";
    int8_t base_value1 = 12;
    char* base_value2 = (char*)malloc(4); strncpy(base_value2, test_string, 4);

    const char* test_string2 = "xyz";
    int8_t value1 = 12;
    char* value2 = (char*)malloc(4); strncpy(value2, test_string2, 4);
    uint64_t value3 = 15;
    auto base_value3 = new Event(_klass);
    base_value3->set_value(_klass_field1.get(), value1);
    base_value3->set_value(_klass_field2.get(), value2);
    base_value3->set_value(_klass_field3.get(), value3);

    base_event.set_value(base_klass_field1.get(), base_value1);
    base_event.set_value(base_klass_field2.get(), base_value2);
    base_event.set_value(base_klass_field3.get(), base_value3);

    // Act
    Event copy_event = base_event;

    // Assert
    ASSERT_EQ(base_value1, copy_event.get_value<int8_t>(base_klass_field1->get_name()));
    ASSERT_STREQ(test_string, copy_event.get_value<char*>(base_klass_field2->get_name()));
    Event* parent = copy_event.get_value<Event*>(base_klass_field3->get_name());
    ASSERT_EQ(value1, parent->get_value<int8_t>(_klass_field1->get_name()));
    ASSERT_STREQ(test_string2, parent->get_value<char*>(_klass_field2->get_name()));
    ASSERT_EQ(value3, parent->get_value<uint64_t>(_klass_field3->get_name()));
}

TEST_F(TestParserEvent, CopyEventWithNullStringShouldSetFieldValueToNull)
{
    // Arrange
    Event event(_klass);
    event.set_value<char*>(_klass_field2.get(), nullptr);

    // Act
    Event event_copy = event;

    // Assert
    ASSERT_EQ(nullptr, event_copy.get_value<char*>(_klass_field2->get_name()));
}

TEST_F(TestParserEvent, CopyEventWithNullEventShouldSetFieldValueToNull)
{
    // Arrange
    auto field = std::make_shared<EventKlassField>("base_field3", "klass_name", FieldTypeId::STRUCT);
    Event event(_klass);
    event.set_value<Event*>(field.get(), nullptr);

    // Act
    Event event_copy = event;

    // Assert
    ASSERT_EQ(nullptr, event_copy.get_value<Event*>(field->get_name()));
}
