#include <client/chrome_trace_converter.hpp>

#include <hawktracer/parser/event.hpp>

#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

using HawkTracer::client::ChromeTraceConverter;
using HawkTracer::parser::EventKlass;
using HawkTracer::parser::Event;
using HawkTracer::parser::FieldType;
using HawkTracer::parser::FieldTypeId;
using HawkTracer::parser::EventKlassField;

namespace {
    static const std::string file_name = "test.log";
}

class TestChromeTraceConverter : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        trace_converter = std::make_shared<ChromeTraceConverter>();
        trace_converter->init(file_name);

        klass_info = std::make_shared<EventKlass>("klass_info", 2);
        klass_value = std::make_shared<EventKlass>("klass_name", 1);

        duration_field = std::make_shared<EventKlassField>("duration", "field_type", FieldTypeId::UINT64);
        info_field = std::make_shared<EventKlassField>("info_klass_id", "field_type", FieldTypeId::UINT32);
        name_field = std::make_shared<EventKlassField>("name", "field_type", FieldTypeId::STRING);
        label_field = std::make_shared<EventKlassField>("label", "field_type", FieldTypeId::STRING);
        mapping_field = std::make_shared<EventKlassField>("event_klass_name", "field_type", FieldTypeId::STRING);
        thread_field = std::make_shared<EventKlassField>("thread_id", "field_type", FieldTypeId::UINT32);
        timestamp_field = std::make_shared<EventKlassField>("timestamp", "field_type", FieldTypeId::UINT64);
        
        first_field = std::make_shared<EventKlassField>("1st", "field_type", FieldTypeId::STRING);
        second_field = std::make_shared<EventKlassField>("2nd", "field_type", FieldTypeId::STRING);
  
    }

    void SetMappingField(Event &event)
    {
        char* value = (char*)malloc(22); strncpy(value, "HT_StringMappingEvent", 22);
        event.set_value(mapping_field.get(), value);
    }

    std::shared_ptr<ChromeTraceConverter> trace_converter;
    
    std::shared_ptr<EventKlass> klass_info;
    std::shared_ptr<EventKlass> klass_value;

    std::shared_ptr<EventKlassField> duration_field;
    std::shared_ptr<EventKlassField> info_field;
    std::shared_ptr<EventKlassField> label_field;
    std::shared_ptr<EventKlassField> mapping_field;
    std::shared_ptr<EventKlassField> name_field;
    std::shared_ptr<EventKlassField> thread_field;
    std::shared_ptr<EventKlassField> timestamp_field;

    std::shared_ptr<EventKlassField> first_field;
    std::shared_ptr<EventKlassField> second_field;

    FieldType value_field;
};

std::string GetFileContents(const std::string file)
{
    std::ifstream file_handle(file, std::ios::in | std::ios::binary);
    std::ostringstream header;
    header << file_handle.rdbuf();
    return header.str();
}

TEST_F(TestChromeTraceConverter, CheckFileAfterOpenButNoEvents)
{   
    trace_converter->stop();

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("]}");

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenEmptyEventProcessed)
{
    Event event(klass_value);
    trace_converter->process_event(event);
    trace_converter->stop();

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("]}");

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenOnlyTimeStampAdded)
{
    Event event(klass_value);

    value_field.f_UINT64 = 12987;
    event.set_value(timestamp_field.get(), value_field);

    trace_converter->process_event(event);
    trace_converter.reset();

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("]}");

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenNameTimeStampAdded)
{
    Event event(klass_info);
    SetMappingField(event);

    // Set info field
    event.set_value(info_field.get(), 123);

    //Process event
    trace_converter->process_event(event);

    Event event_value(klass_value);
    // Add timestamp
    value_field.f_UINT64 = 15987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add name
    char* name_value = (char*)malloc(6); strncpy(name_value, "first", 6);
    event_value.set_value(name_field.get(), name_value);

    trace_converter->process_event(event_value);

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"first\", \"ph\": \"X\", \"ts\": 15, \"dur\": 0, \"pid\": 0, \"tid\": 0, \"args\": {\"name\": \"first\"}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenLabelTimeStampAdded)
{
    Event event(klass_info);
    SetMappingField(event);
    
    // Set info field
    event.set_value(info_field.get(), 456);

    // Process event
    trace_converter->process_event(event);

    Event event_value(klass_value);

    // Add Timestamp
    value_field.f_UINT64 = 16987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add label
    char* label_value = (char*)malloc(6); strncpy(label_value, "second", 6);
    event_value.set_value(label_field.get(), label_value);

    trace_converter->process_event(event_value);

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"second\", \"ph\": \"X\", \"ts\": 16, \"dur\": 0, \"pid\": 0, \"tid\": 0, \"args\": {}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenNameAndDurationAdded)
{
    Event event(klass_info);
    SetMappingField(event);

    // Set info field
    event.set_value(info_field.get(), 789);

    // Process event
    trace_converter->process_event(event);

    Event event_value(klass_value);

    // Add timestamp
    value_field.f_UINT64 = 16987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add name
    char* name_value = (char*)malloc(6); strncpy(name_value, "third", 6);
    event_value.set_value(name_field.get(), name_value);

    // Add duration
    value_field.f_UINT64 = 20000;
    event_value.set_value(duration_field.get(), value_field);

    // Add thread id
    value_field.f_UINT32 = 200;
    event_value.set_value(thread_field.get(), value_field);

    trace_converter->process_event(event_value);

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"third\", \"ph\": \"X\", \"ts\": 16, \"dur\": 20, \"pid\": 0, \"tid\": 200, \"args\": {\"name\": \"third\"}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWhenLabelAndDurationAdded)
{
    Event event(klass_info);
    SetMappingField(event);

    // Add info
    event.set_value(info_field.get(), 123);

    trace_converter->process_event(event);

    Event event_value(klass_value);

    // Add timestamp
    value_field.f_UINT64 = 16987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add label
    char* label_value = (char*)malloc(6); strncpy(label_value, "second", 6);
    event_value.set_value(label_field.get(), label_value);

    // Add duration
    value_field.f_UINT64 = 20000;
    event_value.set_value(duration_field.get(), value_field);

    // Add thread id
    value_field.f_UINT32 = 200;
    event_value.set_value(thread_field.get(), value_field);

    trace_converter->process_event(event_value);

    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"second\", \"ph\": \"X\", \"ts\": 16, \"dur\": 20, \"pid\": 0, \"tid\": 200, \"args\": {}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWithNameAndMultipleArgsAdded)
{
    Event event(klass_info);
    SetMappingField(event);

    // Add info
    event.set_value(info_field.get(), 456);

    trace_converter->process_event(event);

    Event event_value(klass_value);

    // Add timestamp
    value_field.f_UINT64 = 26987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add name
    char* name_value = (char*)malloc(6); strncpy(name_value, "third", 6);
    event_value.set_value(name_field.get(), name_value);

    // Add duration
    value_field.f_UINT64 = 30000;
    event_value.set_value(duration_field.get(), value_field);

    // Add thread id
    value_field.f_UINT32 = 300;
    event_value.set_value(thread_field.get(), value_field);

    // Add first argument
    char* first_value = (char*)malloc(6); strncpy(first_value, "Alpha", 6);
    event_value.set_value(first_field.get(), first_value);

    // Add second argument
    char* second_value = (char*)malloc(5); strncpy(second_value, "Beta", 5);
    event_value.set_value(second_field.get(), second_value);

    trace_converter->process_event(event_value);
    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"third\", \"ph\": \"X\", \"ts\": 26, \"dur\": 30, \"pid\": 0, \"tid\": 300, \"args\": {\"2nd\": \"Beta\",\"1st\": \"Alpha\",\"name\": \"third\"}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}

TEST_F(TestChromeTraceConverter, CheckWithLabelAndMultipleArgsAdded)
{
    Event event(klass_info);
    SetMappingField(event);

    // Add info
    event.set_value(info_field.get(), 456);

    trace_converter->process_event(event);

    Event event_value(klass_value);

    // Add timestamp
    value_field.f_UINT64 = 26987;
    event_value.set_value(timestamp_field.get(), value_field);

    // Add label
    char* label_value = (char*)malloc(6); strncpy(label_value, "fourth", 6);
    event_value.set_value(name_field.get(), label_value);

    // Add duration
    value_field.f_UINT64 = 30000;
    event_value.set_value(duration_field.get(), value_field);

    // Add thread id
    value_field.f_UINT32 = 300;
    event_value.set_value(thread_field.get(), value_field);

    // Add first argument
    char* first_value = (char*)malloc(6); strncpy(first_value, "Alpha", 6);
    event_value.set_value(first_field.get(), first_value);

    // Add second argument
    char* second_value = (char*)malloc(5); strncpy(second_value, "Beta", 5);
    event_value.set_value(second_field.get(), second_value);

    trace_converter->process_event(event_value);
    std::string expected = ChromeTraceConverter::HEADER;
    expected.append("{\"name\": \"fourth\", \"ph\": \"X\", \"ts\": 26, \"dur\": 30, \"pid\": 0, \"tid\": 300, \"args\": {\"2nd\": \"Beta\",\"1st\": \"Alpha\",\"name\": \"fourth\"}}]}");
    trace_converter.reset();

    EXPECT_EQ(expected, GetFileContents(file_name));
}
