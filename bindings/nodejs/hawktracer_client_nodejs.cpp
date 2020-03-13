#include "hawktracer_client_nodejs.hpp"

#include <iostream>
#include <utility>

namespace HawkTracer
{
namespace Nodejs
{

Object Client::init_bindings(class Env env, Object exports)
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

Client::Client(const CallbackInfo &info)
    : ObjectWrap<Client>(info)
{
    _source = info[0].As<String>();
}

Client::~Client()
{
    _stop();
}

Value Client::start(const CallbackInfo &info)
{
    _context_holder->context = ClientContext::create(
        _source,
        [this](std::unique_ptr<std::vector<parser::Event>> data, ClientContext::ConsumeMode consume_mode)
        {
            return handle_events(std::move(data), consume_mode);
        });
    return Boolean::New(info.Env(), static_cast<bool>(_context_holder->context));
}

void Client::stop(const CallbackInfo &)
{
    _stop();
}

void Client::_stop()
{
    std::lock_guard<std::mutex> lock{_callback_mutex};
    if (_callback) {
        _callback->function.Abort();
        _callback->function.Release();
        _callback.reset();
    }
    else {
        delete _context_holder;
    }
}

void Client::set_on_events(const CallbackInfo &info)
{
    std::lock_guard<std::mutex> lock{_callback_mutex};
    if (_callback) {
        // existing _context_holder will be deleted by the finalizer of existing _callback.function
        _context_holder = new ContextHolder{std::move(*_context_holder)};
        _callback->function.Release();
    }
    _callback.reset(new ThreadSafeFunctionHolder{
        ThreadSafeFunction::New(info.Env(),
                                info[0].As<Napi::Function>(),
                                "HawkTracerClientOnEvent",
                                1,
                                1,
                                [](class Env, decltype(_context_holder) context_holder)
                                {
                                    delete context_holder;
                                },
                                _context_holder)});
}

// This method is called from reader thread, while all other methods are called from js main thread.
std::unique_ptr<std::vector<parser::Event>>
Client::handle_events(std::unique_ptr<std::vector<parser::Event>> events, ClientContext::ConsumeMode consume_mode)
{
    std::lock_guard<std::mutex> lock{_callback_mutex};
    if (!_callback) {
        return events;
    }

    // deallocated in convert_and_callback() or below in this method
    auto data = new CallbackDataType{this, std::move(events)};
    napi_status status;
    if (consume_mode == ClientContext::ConsumeMode::FORCE_CONSUME) {
        status = _callback->function.BlockingCall(data, &Client::convert_and_callback);
    }
    else {
        status = _callback->function.NonBlockingCall(data, &Client::convert_and_callback);
    }

    decltype(events) ret{};
    if (status == napi_queue_full) {
        ret = std::move(data->second);
    }
    if (status != napi_ok && status != napi_queue_full) {
        std::cerr << "Request for callback failed with error code: " << status << ", " << data->second->size()
                  << " events are lost." << std::endl;
    }
    if (status != napi_ok) {
        delete data;
    }
    return ret;
}

Value Client::convert_field_value(class Env env, const parser::Event::Value &value)
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
        case parser::FieldTypeId::STRUCT:
            return convert_event(env, *value.value.f_EVENT);
        default:
            assert(0);
    }
}

Object Client::convert_event(class Env env, const parser::Event &event)
{
    auto o = Object::New(env);
    for (const auto &it: event.get_values()) {
        o.Set(it.first, convert_field_value(env, it.second));
    }
    return o;
}

void Client::convert_and_callback(class Env env, Function real_callback, CallbackDataType *data)
{
    std::unique_ptr<CallbackDataType> data_deallocation_guard{data};
    Client *calling_object = data->first;
    std::vector<parser::Event> *events = data->second.get();

    // Prevent Client destruction, which could result in blocking call in handle_events(), which is blocked by
    // real_callback running in js thread, forming deadlock.
    calling_object->Ref();

    Array array = Array::New(env);
    int i = 0;
    std::for_each(events->cbegin(),
                  events->cend(),
                  [env, &array, &i](const parser::Event &e)
                  {
                      array[i++] = convert_event(env, e);
                  });
    real_callback.Call({array});

    calling_object->Unref();
}

} // namespace Nodejs
} // namespace HawkTracer
