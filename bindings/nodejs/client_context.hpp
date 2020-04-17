#ifndef CLIENT_CONTEXT_HPP
#define CLIENT_CONTEXT_HPP

#include <string>

#include "hawktracer/parser/event.hpp"
#include "hawktracer/parser/protocol_reader.hpp"
#include "hawktracer/parser/klass_register.hpp"

namespace HawkTracer
{
namespace Nodejs
{

class ClientContext
{
public:
    using EventCallback = std::function<void()>;
    static std::unique_ptr<ClientContext> create(const std::string& source, EventCallback event_callback);

    ~ClientContext();

    std::vector<parser::Event> take_events();

private:
    ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                  std::unique_ptr<parser::KlassRegister> klass_register,
                  EventCallback event_callback);

    const std::unique_ptr<const parser::KlassRegister> _klass_register; // needs to be destructed after _reader
    const std::unique_ptr<parser::ProtocolReader> _reader;
    const EventCallback _event_callback;

    std::vector<parser::Event> _buffer;
    std::mutex _buffer_mutex;
};

} // namespace Nodejs
} // namespace HawkTracer

#endif //CLIENT_CONTEXT_HPP
