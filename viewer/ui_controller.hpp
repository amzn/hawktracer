#ifndef UI_CONTROLLER_HPP
#define UI_CONTROLLER_HPP

#include "event_db.hpp"
#include "base_ui.hpp"

#include <parser/event.hpp>

#include <unordered_map>
#include <vector>

namespace HawkTracer
{
namespace viewer
{

class UIController
{
public:
    UIController(std::unique_ptr<BaseUI> ui);

    void handle_event(const parser::Event& event);
    void set_time_range(HT_DurationNs duration, HT_TimestampNs stop_ts);
    void request_klass_register();

    std::vector<EventRef> request_data(HT_EventKlassId klass_id);

    TimeRange get_total_ts_range() const;
    TimeRange get_current_ts_range() const;

private:
    void _handle_field_info_event(const parser::Event& event);
    void _update_total_ts_range(const parser::Event& event);
    void _set_timestamp_diff(HT_TimestampNs event_ts);

    std::unique_ptr<BaseUI> _ui;

    HT_TimestampNs _stop_ts;
    HT_DurationNs _duration;

    EventDB _model;

    TimeRange _total_ts_range;
    bool _sync_info_initialized = false;
    std::pair<bool, HT_TimestampNs> _sync_info;
};

} // namespace viewer
} // namespace HawkTracer

#endif // UI_CONTROLLER_HPP
