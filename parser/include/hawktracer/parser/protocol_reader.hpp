#ifndef HAWKTRACER_PARSER_PROTOCOL_READER_HPP
#define HAWKTRACER_PARSER_PROTOCOL_READER_HPP

#include <hawktracer/parser/klass_register.hpp>
#include <hawktracer/parser/event_klass.hpp>
#include <hawktracer/parser/stream.hpp>

#include <atomic>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace HawkTracer
{
namespace parser
{

class ProtocolReader
{
public:
    using OnNewEventCallback = std::function<void(const Event&)>;
    using OnCompleteEventCallback = std::function<void()>;

    ProtocolReader(KlassRegister* klass_register, std::unique_ptr<Stream> stream, bool flat_events);
    ~ProtocolReader();

    void register_events_listener(OnNewEventCallback callback);
    void register_complete_listener(OnCompleteEventCallback callback);

    bool start();
    void stop();

    bool eos() const { return !_is_running; }
    void wait_for_complete();

private:
    void _read_events();
    void _read_event(bool& is_error, Event& event, Event* base_event);
    bool _read_string(FieldType& value);
    bool _read_numeric(FieldType& value, const EventKlassField& field);
    bool _read_struct(FieldType& value, const EventKlassField& field, Event* event, Event* base_event);

    void _call_callbacks(const Event& event);
    void _call_complete_callbacks();

    KlassRegister* _klass_register;
    std::vector<OnNewEventCallback> _on_new_event_callbacks;
    std::vector<OnCompleteEventCallback> _on_complete_event_callbacks;
    std::unique_ptr<Stream> _stream;
    std::thread _thread;
    std::atomic_bool _is_running;
    std::condition_variable _cv;
    std::mutex _mtx_cv;
    bool _flat_events;
    HT_Endianness _endianness = HT_ENDIANNESS_LITTLE;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_PROTOCOL_READER_HPP
