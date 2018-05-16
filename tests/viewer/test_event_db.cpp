#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "mock_cache.hpp"

#include <hawktracer/parser/make_unique.hpp>
#include <viewer/event_db.hpp>

using ::testing::Return;
using ::testing::_;
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
        _event_db = new EventDB(HawkTracer::parser::make_unique<Cache>());
    }

    void TearDown() override
    {
        delete _event_db;
    }

    static void SetUpTestCase() 
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

    static std::shared_ptr<EventKlass> _klass1;
    static std::shared_ptr<EventKlass> _klass2;
    static std::shared_ptr<EventKlassField> _timestamp_field;
    static std::shared_ptr<Event> _event1;
    static std::shared_ptr<Event> _event2;
    static std::shared_ptr<Event> _event3;
    static std::shared_ptr<Event> _event4;
    static FieldType _value1;
    static FieldType _value2;
    static FieldType _value3;
    static FieldType _value4;
    EventDB* _event_db;
};

class TestCacheEventDB : public TestEventDB
{
protected:
    static void SetUpTestCase()
    {
        _value5.f_UINT64 = 250;
        _event5 = std::make_shared<Event>(_klass1);
        _event5.get()->set_value(_timestamp_field.get(), _value5);
    }

    void SetUp() override
    {
        auto _cache = HawkTracer::parser::make_unique<MockCache>();
        _shared_cache = _cache.get();
        _event_db = new EventDB(std::move(_cache));
    }

    void TearDown() override
    {
        delete _event_db;
    }

    MockCache* _shared_cache;
    static std::shared_ptr<Event> _event5;
    static FieldType _value5;
};

std::shared_ptr<EventKlass> TestEventDB::_klass1;
std::shared_ptr<EventKlass> TestEventDB::_klass2;
std::shared_ptr<EventKlassField> TestEventDB::_timestamp_field;
std::shared_ptr<Event> TestEventDB::_event1;
std::shared_ptr<Event> TestEventDB::_event2;
std::shared_ptr<Event> TestEventDB::_event3;
std::shared_ptr<Event> TestEventDB::_event4;
std::shared_ptr<Event> TestCacheEventDB::_event5;
FieldType TestEventDB::_value1;
FieldType TestEventDB::_value2;
FieldType TestEventDB::_value3;
FieldType TestEventDB::_value4;
FieldType TestCacheEventDB::_value5;

TEST_F(TestEventDB, CheckIfEventsAreKeptSorted)
{
    // Arrange
    EventDB _event_db(HawkTracer::parser::make_unique<Cache>());
    Query query;
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 200u;

    // Act
    _event_db.insert(*_event2);
    _event_db.insert(*_event1);
    
    // Assert
    auto response = _event_db.get_data(start_ts, stop_ts, query);
    std::vector<Event> correct_response = {*_event1, *_event2};

    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
        ASSERT_EQ(correct_response[i].get_timestamp(), response[i].get().get_timestamp());
    }
}

TEST_F(TestEventDB, QuerySpecifcKlassId)
{
    // Arrange
    Query query;
    query.klass_id = 1;
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 300u;

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event2);
    _event_db->insert(*_event3);
    auto response = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    std::vector<Event> correct_response = {*_event1, *_event3};
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
        ASSERT_EQ(correct_response[i].get_timestamp(), response[i].get().get_timestamp());
    }
}

TEST_F(TestEventDB, CheckLowerBoundConditions)
{
    // Arrange
    Query query1, query2, query3;
    HT_TimestampNs start_ts;
    HT_TimestampNs stop_ts;

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event2);
    _event_db->insert(*_event3);
    _event_db->insert(*_event4);

    start_ts = 100u;
    stop_ts = 400u;
    auto response1 = _event_db->get_data(start_ts, stop_ts, query1);

    start_ts = 0u;
    stop_ts = 500u;
    auto response2 = _event_db->get_data(start_ts, stop_ts, query2);

    start_ts = 150u;
    stop_ts = 350u;
    auto response3 = _event_db->get_data(start_ts, stop_ts, query3);

    // Assert
    std::vector<Event> correct_response1 = {*_event1, *_event2, *_event3, *_event4};
    std::vector<Event> correct_response2 = {*_event1, *_event2, *_event3, *_event4};
    std::vector<Event> correct_response3 = {*_event2, *_event3};
    ASSERT_EQ(correct_response1.size(), response1.size());
    for (size_t i = 0; i < response1.size(); ++i)
    {
        ASSERT_EQ(correct_response1[i].get_timestamp(), response1[i].get().get_timestamp());
    }
    ASSERT_EQ(correct_response2.size(), response2.size());
    for (size_t i = 0; i < response2.size(); ++i)
    {
        ASSERT_EQ(correct_response2[i].get_timestamp(), response2[i].get().get_timestamp());
    }
    ASSERT_EQ(correct_response3.size(), response3.size());
    for (size_t i = 0; i < response3.size(); ++i)
    {
        ASSERT_EQ(correct_response3[i].get_timestamp(), response3[i].get().get_timestamp());
    }
}

TEST_F(TestEventDB, EmptyDatabaseShouldNotCrash)
{
    // Arrange
    Query query;
    query.klass_id = 1; 
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 200u;

    // Act
    auto response = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    ASSERT_TRUE(response.size() == 0);
}

TEST_F(TestEventDB, QueryWithInvalidTimestampsShouldNotReturnAnything)
{
    // Arrange
    Query query;
    const HT_TimestampNs start_ts = 200u;
    const HT_TimestampNs stop_ts = 100u;

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event2);
    auto response = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    ASSERT_TRUE(response.size() == 0);
}

TEST_F(TestEventDB, QueryOneTimestamp)
{
    // Arrange
    Query query;
    query.klass_id = 1;
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 100u;

    // Act
    _event_db->insert(*_event1);
    auto response = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    std::vector<Event> correct_response = {*_event1};
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
        ASSERT_EQ(correct_response[i].get_timestamp(), response[i].get().get_timestamp());
    }
}

TEST_F(TestCacheEventDB, QueryingTwiceShouldUseCache)
{
    // Arrange
    Query query;
    query.klass_id = 1;
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 300u;

    EXPECT_CALL(*_shared_cache, insert_event(_)).Times(1);
    EXPECT_CALL(*_shared_cache, range_in_cache(start_ts, stop_ts, query.klass_id))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(*_shared_cache, update(query.klass_id, _));
    EXPECT_CALL(*_shared_cache, get_data(query.klass_id, _)).WillOnce(Return(true));

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event3);

    auto response1 = _event_db->get_data(start_ts, stop_ts, query);
    auto response2 = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    std::vector<Event> correct_response1 = {*_event1, *_event3};
    ASSERT_EQ(correct_response1.size(), response1.size());
    for (size_t i = 0; i < response1.size(); ++i)
    {
        ASSERT_EQ(correct_response1[i].get_timestamp(), response1[i].get().get_timestamp());
    }
}

TEST_F(TestCacheEventDB, QueryingDifferentRangeShouldNotUseCache)
{
    // Arrange
    Query query;
    query.klass_id = 1;
    const HT_TimestampNs start_ts1 = 100u;
    const HT_TimestampNs stop_ts1 = 300u;
    const HT_TimestampNs start_ts2 = 200u;
    const HT_TimestampNs stop_ts2 = 400u;

    EXPECT_CALL(*_shared_cache, insert_event(_)).Times(1);
    EXPECT_CALL(*_shared_cache, range_in_cache(start_ts1, stop_ts1, query.klass_id))
        .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*_shared_cache, range_in_cache(start_ts2, stop_ts2, query.klass_id))
            .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*_shared_cache, update(query.klass_id, _)).Times(2);
    EXPECT_CALL(*_shared_cache, get_data(query.klass_id, _)).Times(0);

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event3);

    auto response1 = _event_db->get_data(start_ts1, stop_ts1, query);
    auto response2 = _event_db->get_data(start_ts2, stop_ts2, query);

    // Assert
    std::vector<Event> correct_response1 = {*_event1, *_event3};
    std::vector<Event> correct_response2 = {*_event3};
    ASSERT_EQ(correct_response1.size(), response1.size());
    for (size_t i = 0; i < response1.size(); ++i)
    {
        ASSERT_EQ(correct_response1[i].get_timestamp(), response1[i].get().get_timestamp());
    }
    ASSERT_EQ(correct_response2.size(), response2.size());
    for (size_t i = 0; i < response2.size(); ++i)
    {
        ASSERT_EQ(correct_response2[i].get_timestamp(), response2[i].get().get_timestamp());
    }
}

TEST_F(TestCacheEventDB, QueryingTwiceAndInsertingEventShouldUseCache)
{
    // Arrange
    Query query;
    query.klass_id = 1;
    const HT_TimestampNs start_ts = 100u;
    const HT_TimestampNs stop_ts = 300u;

    EXPECT_CALL(*_shared_cache, insert_event(_)).Times(2);
    EXPECT_CALL(*_shared_cache, range_in_cache(start_ts, stop_ts, query.klass_id))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(*_shared_cache, update(query.klass_id, _)).Times(1);
    EXPECT_CALL(*_shared_cache, get_data(query.klass_id, _)).Times(1);

    // Act
    _event_db->insert(*_event1);
    _event_db->insert(*_event3);
    auto response1 = _event_db->get_data(start_ts, stop_ts, query);

    // Assert
    std::vector<Event> correct_response1 = {*_event1, *_event3};
    ASSERT_EQ(correct_response1.size(), response1.size());
    for (size_t i = 0; i < response1.size(); ++i)
    {
        ASSERT_EQ(correct_response1[i].get_timestamp(), response1[i].get().get_timestamp());
    }

    // Act
    _event_db->insert(*_event5);
    auto response2 = _event_db->get_data(start_ts, stop_ts, query);
}

