#ifndef HAWKTRACER_TIMELINE_H_
#define HAWKTRACER_TIMELINE_H_

#include <hawktracer/config.h>

#include <hawktracer/Vector.h>
#include <hawktracer/Action.h>

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <stack>

namespace hawktracer
{

template<typename ACTION>
struct ActionInfo
{
    typename ACTION::Id id;
    ACTION action;

    ActionInfo()
    {}

    ActionInfo(typename ACTION::Id id, typename ACTION::Label label) :
        id (id)
    {
        action.label = label;
    }

    static bool find_by_id(const typename ACTION::Id& id, const ActionInfo& action_info)
    {
        return id == action_info.id;
    }
};

template<typename ACTION>
class ActionHolder
{
protected:
    ActionHolder() {}

    typename ACTION::Id _start_action(const typename ACTION::Label& label)
    {
        _actions.emplace_back(_next_action_id, label);
        return _next_action_id++;
    }

    ACTION _stop_action(size_t it)
    {
        if (it == _actions.invalid_size)
        {
            return ACTION{};
        }

        ACTION action = _actions[it].action;
        _actions.erase(it);

        return action;
    }

    Vector<ActionInfo<ACTION>> _actions;

    typename ACTION::Id _next_action_id = 1;
};

class Timeline : public ActionHolder<Action>
{
public:
    using ActionStoppedCallback_t = void (*)(Action*, size_t, void*);

    static Timeline& get();

    Action::Id start_action(Action::Label label);
    Action stop_action(Action::Id id);
    Action stop_last_action();

    void push_custom_action(const Action& action);

    static void register_callback(ActionStoppedCallback_t callback, void* user_data);
    static void unregister_callback(ActionStoppedCallback_t callback, void* user_data);

private:
    Timeline();
    ~Timeline();

    Action _stop_and_push_action(NanoTime_t timestamp, size_t index);

    static std::vector<std::pair<ActionStoppedCallback_t, void*>> _callbacks;
    static void _notify_listeners(Action* actions, size_t actions_count);

    static const size_t _buffer_size = HAWKTRACER_TIMELINE_BUFFER_SIZE;
    static std::atomic<int8_t> _thread_id_ctr;
    size_t _buffer_pointer = 0;
    Action buffer[_buffer_size];
    uint8_t _thread_id;
};

} // namespace hawktracer

#endif // HAWKTRACER_TIMELINE_H_
