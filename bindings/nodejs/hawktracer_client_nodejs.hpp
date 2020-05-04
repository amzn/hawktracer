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
    static Object init_bindings(const class Env& env, Object exports);

    explicit Client(const CallbackInfo& info);

    class Value start(const CallbackInfo& info);
    void stop(const CallbackInfo& info);
    void set_on_events(const CallbackInfo& info);

private:
    void _notify_new_event();
    static class Value _convert_field_value(const class Env& env, const parser::Event::Value& value);
    static Object _convert_event(const class Env& env, const LabeledEvent& event);
    static void _convert_and_callback(const class Env& env, Function real_callback, Client* client);

    std::string _source;
    std::string _maps;

    class State
    {
        struct FunctionHolder
        {
            ThreadSafeFunction function;
            ~FunctionHolder()
            {
                function.Release();
            }
        };
        // _client_context states
        // * started: non-null value
        // * stopped: null value
        std::unique_ptr<ClientContext> _client_context;
        // _function_holder states
        // * has_callback: non-null value
        // * no_callback: null value
        std::unique_ptr<FunctionHolder> _function_holder;
        mutable std::mutex _function_holder_mutex;
    public:
        bool is_started() const
        {
            return static_cast<bool>(_client_context);
        }
        // ?         X ?            => started   X ?
        void start(std::unique_ptr<ClientContext> cc)
        {
            _client_context = std::move(cc);
        }
        // ?         X ?            => stopped   X no_callback
        void stop()
        {
            {
                std::lock_guard<std::mutex> lock{_function_holder_mutex};
                if (_function_holder) {
                    _function_holder->function.Abort(); // cancels callbacks already in queue
                    _function_holder.reset();
                }
            }
            _client_context.reset();
        }
        // ?         X ?            => ?         X has_callback
        void set_function(ThreadSafeFunction threadSafeFunction)
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            _function_holder.reset(new FunctionHolder{threadSafeFunction});
        }
        // This method is called from reader thread, while all other methods are called from js main thread.
        napi_status use_function(const std::function<napi_status(ThreadSafeFunction)>& use) const
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            if (!_function_holder)
                return napi_queue_full;

            return use(_function_holder->function);
        }
        std::vector<LabeledEvent> take_events() const
        {
            return _client_context ? _client_context->take_events() : std::vector<LabeledEvent>{};
        }
    };
    State _state;
};

} // namespace Nodejs
} // namespace HawkTracer

static Napi::Object init_bindings(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::Nodejs::Client::init_bindings(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, init_bindings)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
