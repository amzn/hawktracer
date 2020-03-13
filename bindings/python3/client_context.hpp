#ifndef HAWKTRACER_PYTHON_CLIENT_CONTEXT_HPP
#define HAWKTRACER_PYTHON_CLIENT_CONTEXT_HPP

#include <Python.h>

#include "hawktracer/parser/event.hpp"
#include "hawktracer/parser/klass_register.hpp"
#include "hawktracer/parser/protocol_reader.hpp"

#include <mutex>
#include <queue>

namespace HawkTracer
{
namespace Python
{

class Context
{
public:
    Context(std::unique_ptr<parser::Stream> stream, PyObject* on_new_event);
    ~Context();

    void start() { _reader.start(); }
    void stop() { _reader.stop(); }
    PyObject* poll_event();
    bool eos() { return _reader.eos(); }
    void wait_for_complete() { _reader.wait_for_complete(); }

private:
    void _trigger_callback(const parser::Event& event);

    std::mutex _events_mtx;
    std::queue<parser::Event> _events;
    parser::KlassRegister _klass_register;
    parser::ProtocolReader _reader;
    PyObject* _on_new_event = nullptr;
};

} // namespace Python
} // namespace HawkTracer

#endif // HAWKTRACER_PYTHON_CLIENT_CONTEXT_HPP
