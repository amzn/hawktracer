#include <hawktracer/parser/event.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

class TestEvent : public ::testing::Test
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

TEST_F(TestEvent, ValidateSetValueMethod)
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

TEST_F(TestEvent, SetTimestampValueShouldUpdateTimestampField)
{
   // Arrange
    Event event(_klass);
    FieldType value{};
    value.f_UINT64 = 987;
    auto timestamp_field = std::make_shared<EventKlassField>("timestamp", "field_type", FieldTypeId::UINT64);

    // Act
    event.set_value(timestamp_field.get(), value);

    // Assert
    ASSERT_EQ(value.f_UINT64, event.get_timestamp());
}

TEST_F(TestEvent, MergeEventsShouldCopyAllValues)
{
    // Arrange
    Event event1(_klass);
    Event event2(_klass);
    const char* test_string = "aaa";
    FieldType value1{}; value1.f_INT8 = 15;
    FieldType value2{}; value2.f_STRING = (char*)malloc(4); strncpy(value2.f_STRING, test_string, 4);
    FieldType value3{}; value3.f_UINT64 = 15;

    event1.set_value(_klass_field1.get(), value1);
    event2.set_value(_klass_field2.get(), value2);
    event2.set_value(_klass_field3.get(), value3);

    // Act
    event1.merge(std::move(event2));

    // Assert
    ASSERT_EQ(3u, event1.get_values().size());
    ASSERT_EQ(value1.f_INT8, event1.get_value<int8_t>(_klass_field1->get_name()));
    ASSERT_STREQ(test_string, event1.get_value<char*>(_klass_field2->get_name()));
    ASSERT_EQ(value3.f_UINT64, event1.get_value<uint64_t>(_klass_field3->get_name()));
}
