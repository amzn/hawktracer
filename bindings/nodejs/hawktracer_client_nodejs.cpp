#include "hawktracer_client_nodejs.hpp"

#include <iostream>

namespace HawkTracer
{
namespace Nodejs
{

Object Client::init_bindings(const class Env& env, Object exports)
{
    HandleScope scope(env);

    Function constructor = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &Client::start),
            InstanceMethod("onEvents", &Client::set_on_events),
            InstanceMethod("stop", &Client::stop)
        });
    Persistent(constructor).SuppressDestruct();
    exports.Set("HawkTracerClient", constructor);
    return exports;
}

Client::Client(const CallbackInfo& info)
    : ObjectWrap<Client>(info)
{
    _source = info[0].As<String>();
    _maps = info.Length() >= 2 && !info[1].IsUndefined() ? info[1].As<String>() : std::string{};
}

Value Client::start(const CallbackInfo& info)
{
    _state.start(
        ClientContext::create(
            _source,
            _maps,
            [this]()
            {
                _notify_new_event();
            }));
    if (_state.is_started()) {
        Ref();
    }
    else {
        throw Error::New(info.Env(), "Failed to start");
    }
    return Boolean::New(info.Env(), _state.is_started());
}

void Client::stop(const CallbackInfo&)
{
    _state.stop();
    Reset();
}

void Client::set_on_events(const CallbackInfo& info)
{
    // maxQueueSize is set to 2 so that even though the first callback is already running there's room for a new callback.
    // If 2 slots are already filled up, the second callback will pick up the new events with take_events(),
    // in which case we can ignore napi_queue_full.
    _state.set_function(ThreadSafeFunction::New(info.Env(),
                                                info[0].As<Napi::Function>(),
                                                "HawkTracerClientOnEvent",
                                                2,
                                                1));
}

// This method is called from reader thread, while all other methods are called from js main thread.
void Client::_notify_new_event()
{
    auto status = _state.use_function(
        [this](ThreadSafeFunction f)
        {
            return f.NonBlockingCall(this, &Client::_convert_and_callback);
        });

    if (status != napi_ok && status != napi_queue_full) {
        std::cerr << "Request for callback failed with error code: " << status << std::endl;
    }
}

Value Client::_convert_field_value(const class Env& env, const parser::Event::Value& value)
{
    switch (value.field->get_type_id()) {
        case parser::FieldTypeId::UINT8:
            return Number::New(env, value.value.f_UINT8);
        case parser::FieldTypeId::INT8:
            return Number::New(env, value.value.f_INT8);
        case parser::FieldTypeId::UINT16:
            return Number::New(env, value.value.f_UINT16);
        case parser::FieldTypeId::INT16:
            return Number::New(env, value.value.f_INT16);
        case parser::FieldTypeId::UINT32:
            return Number::New(env, value.value.f_UINT32);
        case parser::FieldTypeId::INT32:
            return Number::New(env, value.value.f_INT32);
        case parser::FieldTypeId::UINT64:
            return Number::New(env, value.value.f_UINT64);
        case parser::FieldTypeId::INT64:
            return Number::New(env, value.value.f_INT64);
        case parser::FieldTypeId::STRING:
            return String::New(env, value.value.f_STRING);
        case parser::FieldTypeId::POINTER:
            return String::New(env, "(pointer)");
        default:
            assert(0);
    }
}

Object Client::_convert_event(const class Env& env, const LabeledEvent& event)
{
    auto o = Object::New(env);
    for (const auto& it: event.get_values()) {
        o.Set(it.first, _convert_field_value(env, it.second));
    }
    if (!event.get_label_string().empty()) {
        o.Set("label", String::New(env, event.get_label_string()));
    }
    return o;
}

void Client::_convert_and_callback(const class Env& env, Function real_callback, Client* calling_object)
{
    std::vector<LabeledEvent> events = calling_object->_state.take_events();

    if (!events.empty()) {
        Array array = Array::New(env);
        int i = 0;
        std::for_each(events.cbegin(),
                      events.cend(),
                      [env, &array, &i](const LabeledEvent& e)
                      {
                          array[i++] = _convert_event(env, e);
                      });
        real_callback.Call({array});
    }
}

} // namespace Nodejs
} // namespace HawkTracer
