#include "base_ui.hpp"
#include "ui_controller.hpp"

#include <cassert>

namespace HawkTracer
{
namespace viewer
{

void BaseUI::set_controller(UIController* controller)
{
    assert(controller);
    _controller = controller;
}

void BaseUI::_set_time_range(HT_DurationNs duration, HT_TimestampNs stop_ts)
{
    _controller->set_time_range(duration, stop_ts);
}

bool BaseUI::_track_field(HT_EventKlassId klass_id, const char* field_name)
{
    return _controller->track_field(klass_id, field_name);
}

void BaseUI::_request_klass_register()
{
    _controller->request_klass_register();
}

void BaseUI::_request_data()
{
    _controller->request_data();
}

TimeRange BaseUI::_get_total_ts_range() const
{
    return _controller->get_total_ts_range();
}

TimeRange BaseUI::_get_current_ts_range() const
{
    return _controller->get_current_ts_range();
}

} // namespace viewer
} // namespace HawkTracer
