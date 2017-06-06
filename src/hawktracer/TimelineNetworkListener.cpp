#include "TimelineNetworkListener.h"
#include "config.h"

#include <cstring>
#include <unistd.h>

namespace hawktracer
{

TimelineNetworkListener::TimelineNetworkListener(int port)
{
    if (_tcp_server.start(port))
    {
        _sender_thread = std::thread([this] { _process_queue(); });
    }
    else
    {
        HAWKTRACER_PRINT("Cannot start tcp client\n");
    }
}

TimelineNetworkListener::~TimelineNetworkListener()
{
    _tcp_server.stop();
    _push_data_queue_cv.notify_all();
    _sender_thread.join();
}

void TimelineNetworkListener::_process_queue()
{
    std::unique_lock<std::mutex> l(_data_queue_mutex);
    while (true)
    {
        _push_data_queue_cv.wait(l, [this] { return !_data_queue.empty() || !_tcp_server.is_running(); });

        if (!_tcp_server.is_running())
        {
            break;
        }

        char buffer[_max_buffer_size * Action::STRUCT_SIZE];

        size_t ptr = _data_queue.pop();

        if (!_tcp_server.client_count())
        {
            continue;
        }

        size_t bufSize = 0;
        for (size_t i = 0; i < _max_buffer_size; i++)
        {
            _buffer[ptr][i].serialize(buffer + bufSize, _max_buffer_size * Action::STRUCT_SIZE - bufSize);
            bufSize += Action::STRUCT_SIZE;
        }

        l.unlock();

        _tcp_server.write(buffer, bufSize);

        l.lock();
    }
}

void TimelineNetworkListener::_push_action(const Action& action)
{
    _buffer[_buffer_number][_buffer_pointer++] = action;

    if (_buffer_pointer == _max_buffer_size)
    {
        _send_current_buffer();
        _buffer_number = (_buffer_number + 1) % _buffer_count;
        _buffer_pointer = 0;
    }
}

void TimelineNetworkListener::_push_actions(Action* actions, size_t actions_count)
{
    if (!_tcp_server.is_running())
    {
        return;
    }

    std::lock_guard<std::mutex> l(_push_action_mutex);

    for (size_t i = 0; i < actions_count; i++)
    {
        _push_action(actions[i]);
    }
}

void TimelineNetworkListener::_send_current_buffer()
{
    std::lock_guard<std::mutex> l(_data_queue_mutex);

    _data_queue.push(_buffer_number);
    _push_data_queue_cv.notify_all();
}

void TimelineNetworkListener::timeline_network_listener_callback(Action* actions, size_t actions_count, void* user_data)
{
    reinterpret_cast<TimelineNetworkListener*>(user_data)->_push_actions(actions, actions_count);
}

} // namespace hawktracer
