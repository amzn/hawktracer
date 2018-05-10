#include <gtest/gtest.h>
#include <viewer/event_db.hpp>

using namespace HawkTracer::viewer;
using HawkTracer::parser::Event;
using HawkTracer::parser::EventKlass;
using HawkTracer::parser::EventKlassField;
using HawkTracer::parser::FieldTypeId;
using HawkTracer::parser::FieldType;

class TestEventDB : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        // Setup Events Klasses 
        _klass1 = std::make_shared<EventKlass>("first_klass", 1);
        _klass2 = std::make_shared<EventKlass>("second_klass", 2);

        // Setup timestamp field for events
        _timestamp_field = std::make_shared<EventKlassField>("timestamp", "HT_TimestampNs", FieldTypeId::UINT64);

        // Setup values for event's timestamps
        _value1.f_UINT64 = 100;
        _value2.f_UINT64 = 200;
        _value3.f_UINT64 = 300;
        _value4.f_UINT64 = 400;

        // Setup events
        _event1 = std::make_shared<Event>(_klass1);
        _event2 = std::make_shared<Event>(_klass2);
        _event3 = std::make_shared<Event>(_klass1);
        _event4 = std::make_shared<Event>(_klass2);

        // Set timestamps for events
        _event1.get()->set_value(_timestamp_field.get(), _value1);
        _event2.get()->set_value(_timestamp_field.get(), _value2);
        _event3.get()->set_value(_timestamp_field.get(), _value3); 
        _event4.get()->set_value(_timestamp_field.get(), _value4);
    }

    std::shared_ptr<EventKlass> _klass1;
    std::shared_ptr<EventKlass> _klass2;
    std::shared_ptr<EventKlassField> _timestamp_field;
    std::shared_ptr<Event> _event1;
    std::shared_ptr<Event> _event2;
    std::shared_ptr<Event> _event3;
    std::shared_ptr<Event> _event4;
    FieldType _value1{};
    FieldType _value2{};
    FieldType _value3{};
    FieldType _value4{};
    EventDB event_db;
};

TEST_F(TestEventDB, CheckIfEventsAreKeptSorted)
{
    // Arrange
    Query query;
    HT_TimestampNs start_ts = (HT_TimestampNs)100;
    HT_TimestampNs stop_ts = (HT_TimestampNs)200;

    // Act
    event_db.insert(*_event2);
    event_db.insert(*_event1);
    
    // Assert
    auto response = event_db.get_data(start_ts, stop_ts, query);
    std::vector<Event> correct_response = {*_event1, *_event2};

    ASSERT_EQ(response.size(), correct_response.size());
    for (unsigned int i = 0; i < response.size(); ++i)
    {
        ASSERT_EQ(response[i].get().get_timestamp(), correct_response[i].get_timestamp());
    }
}

TEST_F(TestEventDB, QuerySpecifcKlassId)
{
    // Arrange
    Query query;
    HT_TimestampNs start_ts = (HT_TimestampNs)100;
    HT_TimestampNs stop_ts = (HT_TimestampNs)300;
    query.klass_id = 1;

    // Act
    event_db.insert(*_event1);
    event_db.insert(*_event2);
    event_db.insert(*_event3);
    auto response = event_db.get_data(start_ts, stop_ts, query);

    // Assert
    std::vector<Event> correct_response = {*_event1, *_event3};
    ASSERT_EQ(response.size(), correct_response.size());
    for (unsigned int i = 0; i < response.size(); ++i)
    {
        ASSERT_EQ(response[i].get().get_timestamp(), correct_response[i].get_timestamp());
    }
}


TEST_F(TestEventDB, CheckLowerBoundConditions)
{
    // Arrange
    Query query1, query2, query3;
    HT_TimestampNs start_ts;
    HT_TimestampNs stop_ts;

    // Act
    event_db.insert(*_event1);
    event_db.insert(*_event2);
    event_db.insert(*_event3);
    event_db.insert(*_event4);

    start_ts = (HT_TimestampNs)100;
    stop_ts = (HT_TimestampNs)400;
    auto response1 = event_db.get_data(start_ts, stop_ts, query1);

    start_ts = (HT_TimestampNs)0;
    stop_ts = (HT_TimestampNs)500;
    auto response2 = event_db.get_data(start_ts, stop_ts, query2);

    start_ts = (HT_TimestampNs)150;
    stop_ts = (HT_TimestampNs)350;
    auto response3 = event_db.get_data(start_ts, stop_ts, query3);

    // Assert
    std::vector<Event> correct_response1 = {*_event1, *_event2, *_event3, *_event4};
    std::vector<Event> correct_response2 = {*_event1, *_event2, *_event3, *_event4};
    std::vector<Event> correct_response3 = {*_event2, *_event3};
    ASSERT_EQ(response1.size(), correct_response1.size());
    for (unsigned int i = 0; i < response1.size(); ++i)
    {
        ASSERT_EQ(response1[i].get().get_timestamp(), correct_response1[i].get_timestamp());
    }
    ASSERT_EQ(response2.size(), correct_response2.size());
    for (unsigned int i = 0; i < response2.size(); ++i)
    {
        ASSERT_EQ(response2[i].get().get_timestamp(), correct_response2[i].get_timestamp());
    }
    ASSERT_EQ(response3.size(), correct_response3.size());
    for (unsigned int i = 0; i < response3.size(); ++i)
    {
        ASSERT_EQ(response3[i].get().get_timestamp(), correct_response3[i].get_timestamp());
    }
}

