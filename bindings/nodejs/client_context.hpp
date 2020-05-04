#ifndef CLIENT_CONTEXT_HPP
#define CLIENT_CONTEXT_HPP

#include <string>

#include "hawktracer/client_utils/converter.hpp"
#include "hawktracer/parser/event.hpp"
#include "hawktracer/parser/protocol_reader.hpp"
#include "hawktracer/parser/klass_register.hpp"

namespace HawkTracer
{
namespace Nodejs
{

class LabeledEvent: public parser::Event
{
public:
    LabeledEvent(const Event& event, std::string label_string)
        : Event(event), _label_string(std::move(label_string))
    {}
    std::string get_label_string() const { return _label_string; }
private:
    std::string _label_string;
};

class ClientContext
{
public:
    using EventCallback = std::function<void()>;
    static std::unique_ptr<ClientContext>
    create(const std::string& source, const std::string& map_files, EventCallback event_callback);

    ~ClientContext();

    std::vector<LabeledEvent> take_events();

private:
    ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                  std::unique_ptr<parser::KlassRegister> klass_register,
                  std::unique_ptr<ClientUtils::Converter> converter,
                  EventCallback event_callback);

    const std::unique_ptr<const parser::KlassRegister> _klass_register; // needs to be destructed after _reader
    const std::unique_ptr<parser::ProtocolReader> _reader;
    const std::unique_ptr<ClientUtils::Converter> _label_mapper;
    const EventCallback _event_callback;

    std::vector<LabeledEvent> _buffer;
    std::mutex _buffer_mutex;
};

} // namespace Nodejs
} // namespace HawkTracer

#endif //CLIENT_CONTEXT_HPP
