#include "client_context.hpp"
#include "hawktracer/parser/event.hpp"

namespace HawkTracer
{
namespace Python
{

static PyObject* event_to_dictionary(const parser::Event& event)
{
#define CREATE_INT_TYPE(HAWKTRACER_TYPE, C_TYPE) \
    case parser::FieldTypeId::HAWKTRACER_TYPE: \
    val = PyLong_FromUnsignedLongLong((C_TYPE)p.second.value.f_##HAWKTRACER_TYPE); \
    break;

    PyObject* dict = PyDict_New();

    for (const auto& p : event.get_values())
    {
        PyObject* val = nullptr;
        switch (p.second.field->get_type_id())
        {
        MKCREFLECT_FOREACH(CREATE_INT_TYPE, uint64_t, UINT8, UINT16, UINT32, UINT64, POINTER)
        MKCREFLECT_FOREACH(CREATE_INT_TYPE, int64_t, INT8, INT16, INT32, INT64)
#undef CREATE_INT_TYPE
        case parser::FieldTypeId::STRING:
            val = PyUnicode_FromString(p.second.value.f_STRING);
            break;
        default:
            break;
        }

        if (!val)
        {
            continue;
        }

        PyDict_SetItem(dict, PyUnicode_FromString(p.first.c_str()), val);
    }

    return dict;
}

Context::Context(std::unique_ptr<parser::Stream> stream, PyObject* on_new_event) :
    _reader(&_klass_register, std::move(stream), true),
    _on_new_event(on_new_event)
{
    if (_on_new_event)
    {
        Py_XINCREF(_on_new_event);
    }
    _reader.register_events_listener([this] (const parser::Event& event) {
        if (_on_new_event)
        {
            _trigger_callback(event);
        }
        else
        {
            std::lock_guard<std::mutex> l(_events_mtx);
            _events.push(event);
        }
    });
}

Context::~Context()
 {
     if (_on_new_event)
     {
         Py_DECREF(_on_new_event);
     }
 }

PyObject* Context::poll_event()
{
    std::lock_guard<std::mutex> l(_events_mtx);
    if (_events.empty())
    {
        Py_RETURN_NONE;
    }

    parser::Event evt = _events.front();
    PyObject* py_event = event_to_dictionary(evt);
    _events.pop();
    return Py_BuildValue("sO", evt.get_klass()->get_name().c_str(), py_event);
}

void Context::_trigger_callback(const parser::Event& event)
{
    PyObject* arglist = Py_BuildValue("(sO)", event.get_klass()->get_name().c_str(), event_to_dictionary(event));
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* result = PyEval_CallObject(_on_new_event, arglist);
    Py_DECREF(arglist);

    if (!result)
    {
        printf("Failed to call callback! ");
        printf("Callback must have 2 arguments: (klass_name, event_data)\n");
    }
    else
    {
        Py_DECREF(result);
    }

    PyGILState_Release(gstate);
}

} // namespace Python
} // namespace HawkTracer
