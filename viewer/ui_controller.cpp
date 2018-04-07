#include "ui_controller.hpp"
#include "logger.hpp"

#include <hawktracer/monotonic_clock.h>
#include <parser/klass_register.hpp>
#include <cstring>
#include <algorithm>

namespace HawkTracer
{
namespace viewer
{

using namespace parser;

UIController::UIController(std::unique_ptr<BaseUI> ui) :
    _ui(std::move(ui)),
    _total_ts_range((HT_TimestampNs)-1, 0)
{
    _ui->set_controller(this);
}

void UIController::handle_event(const Event& event)
{
    const auto& klass = event.get_klass();

    switch (static_cast<WellKnownKlasses>(klass->get_id()))
    {
    case WellKnownKlasses::EventKlassInfoEventKlass:
        _ui->add_klass(KlassRegister::get().get_klass(event.get_value<uint32_t>("info_klass_id")).get());
        _set_timestamp_diff(event.get_timestamp());
        break;
    case WellKnownKlasses::EventKlassFieldInfoEventKlass:
        _handle_field_info_event(event);
        break;
    default:
        // We don't need register events in a model
        _model.insert(event);
        _update_total_ts_range(event);
    }
}

void UIController::_handle_field_info_event(const Event& event)
{
    const auto& klass = KlassRegister::get().get_klass(event.get_value<uint32_t>("info_klass_id"));
    const char* field_name = event.get_value<char*>("field_name");
    std::shared_ptr<const EventKlassField> field = klass->get_field(field_name, true);

    if (field)
    {
        _ui->add_field(klass.get(), field.get());
    }
    else
    {
        Logger::log("Field '" + std::string(field_name) + "' doesn't exist in a klass " + klass->get_name());
    }
}

void UIController::set_time_range(HT_DurationNs duration, HT_TimestampNs stop_ts)
{
    if (duration > stop_ts)
    {
        Logger::log("Duration larger than stop_ts, set duration to stop_ts");
        duration = stop_ts;
    }

    _duration = duration;
    _stop_ts = stop_ts;
}

void UIController::request_klass_register()
{
    for (const auto& klass_it : KlassRegister::get().get_klasses())
    {
        const auto& klass = klass_it.second;
        _ui->add_klass(klass.get());
        for (const auto& field: klass->get_fields())
        {
            _ui->add_field(klass.get(), field.get());
        }
    }
}

std::vector<EventRef> UIController::request_data(HT_EventKlassId klass_id)
{
    auto range = get_current_ts_range();
    return _model.get_data(range.start, range.stop, klass_id);
}

TimeRange UIController::get_total_ts_range() const
{
    return _total_ts_range;
}

TimeRange UIController::get_current_ts_range() const
{
    auto stop_ts = _stop_ts;
    if (stop_ts == (HT_TimestampNs)-1)
    {
        auto now_ts = ht_monotonic_clock_get_timestamp() ;
        stop_ts = _sync_info.first ? now_ts - _sync_info.second : now_ts + _sync_info.second;
    }

    std::cout << "Current range: " << stop_ts - _duration << " " << stop_ts << std::endl;

    return TimeRange(stop_ts - _duration, stop_ts);
}

void UIController::_update_total_ts_range(const Event& event)
{
    auto ts = event.get_timestamp();
    if (ts < _total_ts_range.start)
    {
        _total_ts_range.start = ts;
    }
    if (ts > _total_ts_range.stop)
    {
        _total_ts_range.stop = ts;
    }
}

void UIController::_set_timestamp_diff(HT_TimestampNs event_ts)
{
    if (!_sync_info_initialized)
    {
        // TODO: this sync method is kind of a joke, but it works
        // TODO: implement sync properly!
        _sync_info_initialized = true;
        HT_TimestampNs current_ts = ht_monotonic_clock_get_timestamp();
        if (current_ts > event_ts)
        {
            _sync_info = std::make_pair(true, current_ts - event_ts);
        }
        else
        {
            _sync_info = std::make_pair(false, event_ts - current_ts);
        }
    }
}

} // namespace viewer
} // namespace HawkTracer
