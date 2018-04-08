#ifndef HAWKTRACER_VIEWER_BASE_UI_HPP
#define HAWKTRACER_VIEWER_BASE_UI_HPP

#include "event_db.hpp"

#include <parser/event.hpp>
#include <hawktracer/base_types.h>

namespace HawkTracer
{
namespace viewer
{

class UIController;

struct TimeRange
{
    TimeRange(HT_TimestampNs start, HT_TimestampNs stop) :
       start(start), stop(stop)
    {
    }

    HT_DurationNs get_duration() const { return stop - start; }

    HT_TimestampNs start;
    HT_TimestampNs stop;
};

class BaseUI
{
public:
    virtual ~BaseUI() {}

    void set_controller(UIController* controller);

    virtual void add_klass(const parser::EventKlass* klass) = 0;
    virtual void add_field(const parser::EventKlass* klass, const parser::EventKlassField* field) = 0;
    virtual int run() = 0;

protected:
    void _set_time_range(HT_DurationNs duration, HT_TimestampNs stop_ts);
    std::vector<EventRef> _request_data(const Query& query);
    void _request_klass_register();
    TimeRange _get_total_ts_range() const;
    TimeRange _get_current_ts_range() const;

private:
    UIController* _controller;
};

} // namespace viewer
} // namespace HawkTracer

#endif // HAWKTRACER_VIEWER_BASE_UI_HPP
