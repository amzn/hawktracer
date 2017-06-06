#include "Timeline.h"

namespace hawktracer
{

std::atomic<int8_t> Timeline::_thread_id_ctr(0);

std::vector<std::pair<Timeline::ActionStoppedCallback_t, void*>> Timeline::_callbacks;

Timeline& Timeline::get()
{
    thread_local static Timeline timeline;

    return timeline;
}

Timeline::Timeline()
{
    _thread_id = _thread_id_ctr++;
}

Timeline::~Timeline()
{
    if (_buffer_pointer > 0)
    {
        _notify_listeners(buffer, _buffer_pointer);
    }
}

Action::Id Timeline::start_action(Action::Label label)
{
    auto id = _start_action(label);
    _actions[_actions.size() - 1].id = id;
    return id;
}

Action Timeline::stop_action(Action::Id id)
{
    NanoTime_t current_time = Action::get_time_now_nanoseconds();

    return _stop_and_push_action(current_time, _actions.find(id, ActionInfo<Action>::find_by_id));
}

Action Timeline::stop_last_action()
{
    NanoTime_t current_time = Action::get_time_now_nanoseconds();

    return _stop_and_push_action(current_time, _actions.size() - 1);
}

void Timeline::push_custom_action(const Action& action)
{
    buffer[_buffer_pointer++] = action;

    if (_buffer_pointer == _buffer_size)
    {
        _notify_listeners(buffer, _buffer_size);
        _buffer_pointer = 0;
    }
}

Action Timeline::_stop_and_push_action(NanoTime_t timestamp, size_t index)
{
    NanoTime_t currentTime = Action::get_time_now_nanoseconds();

    Action action = _stop_action(index);
    action.stopTime = currentTime;
    action.threadId = _thread_id;

    push_custom_action(action);

    return action;
}

void Timeline::_notify_listeners(Action* actions, size_t actions_count)
{
    for (const auto& callback : _callbacks)
    {
        callback.first(actions, actions_count, callback.second);
    }
}

void Timeline::register_callback(ActionStoppedCallback_t callback, void* user_data)
{
    _callbacks.push_back(std::make_pair(callback, user_data));
}

void Timeline::unregister_callback(ActionStoppedCallback_t callback, void* user_data)
{
    for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
    {
        if (it->first == callback && it->second == user_data)
        {
            _callbacks.erase(it);
            break;
        }
    }
}

} // namespace hawktracer
