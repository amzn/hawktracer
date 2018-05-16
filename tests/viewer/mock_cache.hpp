#include <gmock/gmock.h>

#include <viewer/icache.hpp>

using HawkTracer::viewer::ICache;
using HawkTracer::viewer::EventRef;

class MockCache : public ICache
{
public:
    MOCK_METHOD3(range_in_cache, bool(HT_TimestampNs start_ts,
                                      HT_TimestampNs stop_ts,
                                      HT_EventKlassId klass_id));

    MOCK_METHOD2(get_data, bool(HT_EventKlassId klass_id,
                                std::vector<EventRef>& events));

    MOCK_METHOD2(update, void(HT_EventKlassId klass_id,
                              std::vector<EventRef> new_response));

    MOCK_METHOD1(insert_event, void(EventRef event));
};

