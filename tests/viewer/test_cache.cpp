#include <gtest/gtest.h>
#include <viewer/cache.hpp>

using namespace HawkTracer::viewer;
using HawkTracer::parser::Event;
using HawkTracer::parser::EventKlass;
using HawkTracer::parser::EventKlassField;
using HawkTracer::parser::FieldTypeId;
using HawkTracer::parser::FieldType;

class TestCache : public ::testing::Test
{
protected:
    static void SetUpTestCase() 
    {
        // Setup Events Klasses 
        _klass1 = std::make_shared<EventKlass>("first_klass", 1);

        // Setup timestamp field for events
        _timestamp_field = std::make_shared<EventKlassField>("timestamp", "HT_TimestampNs", FieldTypeId::UINT64);

        // Setup values for event's timestamps
        _value1.f_UINT64 = 100;
        _value2.f_UINT64 = 200;
        _value3.f_UINT64 = 300;

        // Setup events
        _event1 = std::make_shared<Event>(_klass1);
        _event2 = std::make_shared<Event>(_klass1);
        _event3 = std::make_shared<Event>(_klass1);

        // Set timestamps for events
        _event1.get()->set_value(_timestamp_field.get(), _value1);
        _event2.get()->set_value(_timestamp_field.get(), _value2);
        _event3.get()->set_value(_timestamp_field.get(), _value3); 
    }

    static std::shared_ptr<EventKlass> _klass1;
    static std::shared_ptr<EventKlassField> _timestamp_field;
    static std::shared_ptr<Event> _event1;
    static std::shared_ptr<Event> _event2;
    static std::shared_ptr<Event> _event3;
    static FieldType _value1;
    static FieldType _value2;
    static FieldType _value3;

    Cache _cache;
};

std::shared_ptr<EventKlass> TestCache::_klass1;
std::shared_ptr<EventKlassField> TestCache::_timestamp_field;
std::shared_ptr<Event> TestCache::_event1;
std::shared_ptr<Event> TestCache::_event2;
std::shared_ptr<Event> TestCache::_event3;
FieldType TestCache::_value1;
FieldType TestCache::_value2;
FieldType TestCache::_value3;

TEST_F(TestCache, TestUpdateAndGetData)
{
    // Arrange
    std::vector<EventRef> events = {*_event1, *_event3};
    const HT_EventKlassId klass_id = 1;
    _cache.update(klass_id, events);

    // Act
    std::vector<EventRef> response;
    bool data_found = _cache.get_data(klass_id, response);
    
    // Assert
    std::vector<EventRef> correct_response{*_event1, *_event3};

    ASSERT_TRUE(data_found == true);
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
       ASSERT_EQ(correct_response[i].get().get_timestamp(), response[i].get().get_timestamp()); 
    }
}

TEST_F(TestCache, UpdatingCacheWithEmptyVectorShouldNotUpdate)
{
    // Arrange
    std::vector<EventRef> empty_vector;
    const HT_EventKlassId klass_id = 1;
    _cache.update(klass_id, empty_vector);

    // Act
    std::vector<EventRef> response;
    bool data_found = _cache.get_data(klass_id, response);
    
    // Assert
    ASSERT_TRUE(data_found == false);
}

TEST_F(TestCache, TestInsertEventForExistingKlassInCache)
{
    // Arrange
    std::vector<EventRef> events = {*_event1, *_event3};
    const HT_EventKlassId klass_id = 1;
    _cache.update(klass_id, events);

    // Act
    _cache.insert_event(*_event2); 
    
    // Assert
    std::vector<EventRef> response;
    bool data_found = _cache.get_data(klass_id, response);
    std::vector<EventRef> correct_response{*_event1, *_event2, *_event3};

    ASSERT_TRUE(data_found == true);
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
       ASSERT_EQ(correct_response[i].get().get_timestamp(), response[i].get().get_timestamp()); 
    }
}

TEST_F(TestCache, TestInsertEventForNonExistingKlassInCache)
{
    // Arrange
    std::vector<EventRef> events = {*_event1};
    const HT_EventKlassId klass_id1 = 1;
    _cache.update(klass_id1, events);

    // Act
    _cache.insert_event(*_event3);
    
    // Assert
    std::vector<EventRef> response;
    bool data_found = _cache.get_data(klass_id1, response);
    std::vector<EventRef> correct_response{*_event1};

    ASSERT_TRUE(data_found == true);
    ASSERT_EQ(correct_response.size(), response.size());
    for (size_t i = 0; i < response.size(); ++i)
    {
       ASSERT_EQ(correct_response[i].get().get_timestamp(), response[i].get().get_timestamp()); 
    }
}

TEST_F(TestCache, TestRangeInCache)
{
    // Arrange
    const HT_TimestampNs start_ts1 = 100u;
    const HT_TimestampNs stop_ts1 = 300u;
    const HT_TimestampNs start_ts2 = 400u;
    const HT_TimestampNs stop_ts2 = 500u;
    std::vector<EventRef> events = {*_event1, *_event3};
    _cache.update(1, events);

    // Act
    bool response1 = _cache.range_in_cache(start_ts1, stop_ts1, 3);
    bool response2 = _cache.range_in_cache(start_ts1, stop_ts1, 1);
    bool response3 = _cache.range_in_cache(start_ts2, stop_ts2, 1);
    bool response4 = _cache.range_in_cache(stop_ts1, start_ts1, 1);
    
    // Assert
    bool correct_response1 = false;
    bool correct_response2 = true;
    bool correct_response3 = false;
    bool correct_response4 = false;

    ASSERT_EQ(correct_response1, response1);
    ASSERT_EQ(correct_response2, response2);
    ASSERT_EQ(correct_response3, response3);
    ASSERT_EQ(correct_response4, response4);
}

