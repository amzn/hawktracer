#ifndef HAWKTRACER_CLIENT_NODEJS_HPP
#define HAWKTRACER_CLIENT_NODEJS_HPP

#include "client_context.hpp"

#include <napi.h>

namespace HawkTracer
{
namespace Nodejs
{

using namespace Napi;

class Client: public ObjectWrap<Client>
{
public:
    static Object init_bindings(class Env env, Object exports);

    explicit Client(const CallbackInfo &info);
    ~Client() override;

    class Value start(const CallbackInfo &info);
    void stop(const CallbackInfo &info);
    void set_on_events(const CallbackInfo &info);

private:
    void _stop();
    std::unique_ptr<std::vector<parser::Event>>
    handle_events(std::unique_ptr<std::vector<parser::Event>> events, ClientContext::ConsumeMode consume_mode);
    static class Value convert_field_value(class Env env, const parser::Event::Value &value);
    static Object convert_event(class Env env, const parser::Event &event);

    using CallbackDataType = std::pair<Client*, std::unique_ptr<std::vector<parser::Event>>>;
    static void convert_and_callback(class Env env, Function real_callback, CallbackDataType *data);

    std::string _source {};

    struct ContextHolder
    {
        std::unique_ptr<ClientContext> context{};
    };
    // deallocated in the finalizer of _callback.function set up in set_on_events(), or _stop()
    ContextHolder *_context_holder {new ContextHolder {}};

    struct ThreadSafeFunctionHolder
    {
        ThreadSafeFunction function;
    };
    std::unique_ptr<ThreadSafeFunctionHolder> _callback {};
    std::mutex _callback_mutex {};
};

} // namespace Nodejs
} // namespace HawkTracer

static Napi::Object init_bindings(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::Nodejs::Client::init_bindings(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, init_bindings)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
