#ifndef HAWKTRACER_TIMELINENETWORKLISTENER_H_
#define HAWKTRACER_TIMELINENETWORKLISTENER_H_

#include <hawktracer/config.h>

#include <hawktracer/CircularBuffer.h>
#include <hawktracer/Timeline.h>
#include <hawktracer/TCPServer.h>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace hawktracer
{

class TimelineNetworkListener
{
public:
    TimelineNetworkListener(int port);
    virtual ~TimelineNetworkListener();

    static void timeline_network_listener_callback(Action* actions, size_t action_count, void* user_data);

private:
    void _push_action(const Action& action);
    void _push_actions(Action* actions, size_t actions_count);
    void _send_current_buffer();
    void _process_queue();

    static size_t _serialize_buffer(const Action& buffer, char* outBuffer);

    static const size_t _max_buffer_size = HAWKTRACER_TIMELINE_NETWORK_LISTENER_MAX_BUFFER_SIZE;
    static const size_t _buffer_count = HAWKTRACER_TIMELINE_NETWORK_LISTENER_BUFFER_COUNT;

    Action _buffer[_buffer_count][_max_buffer_size];
    size_t _buffer_pointer = 0;
    size_t _buffer_number = 0;

    std::mutex _push_action_mutex;

    CircularBuffer<size_t, _buffer_count> _data_queue;
    std::mutex _data_queue_mutex;
    std::condition_variable _push_data_queue_cv;

    TCPServer _tcp_server;

    std::thread _sender_thread;
};

} // namespace hawktracer

#endif // HAWKTRACER_TIMELINENETWORKLISTENER_H_
