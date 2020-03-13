#include "client_context.hpp"

#include "hawktracer/client_utils/stream_factory.hpp"
#include "hawktracer/parser/make_unique.hpp"
#include <iostream>
#include <utility>

namespace HawkTracer
{
namespace Nodejs
{

std::unique_ptr<ClientContext> ClientContext::create(const std::string &source, EventCallback event_callback)
{
    std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source);
    if (!stream) {
        return nullptr;
    }

    std::unique_ptr<parser::KlassRegister> klass_register{new parser::KlassRegister()};
    std::unique_ptr<parser::ProtocolReader>
        reader{new parser::ProtocolReader(klass_register.get(), std::move(stream), true)};
    std::unique_ptr<ClientContext>
        context{new ClientContext(std::move(reader), std::move(klass_register), std::move(event_callback))};
    return context;
}

ClientContext::ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                             std::unique_ptr<parser::KlassRegister> klass_register,
                             EventCallback event_callback)
    : _reader(std::move(reader)), _klass_register(std::move(klass_register)), _event_callback(std::move(event_callback))
{
    _reader->register_events_listener(
        [this](const parser::Event &event)
        {
            if (!_events) {
                _events.reset(new std::vector<parser::Event>{});
            }
            _events->push_back(event);
            _events = _event_callback(std::move(_events), ConsumeMode::TRY_CONSUME);
        });
    _reader->register_complete_listener(
        [this]()
        {
            if (!_events || _events->empty()) {
                return;
            }
            _events = _event_callback(std::move(_events), ConsumeMode::FORCE_CONSUME);
            if (_events && !_events->empty()) {
                std::cerr << _events->size() << " events were not processed." << std::endl;
            }
        });

    _reader->start();
}

ClientContext::~ClientContext()
{
    _reader->stop();
}

} // namespace Nodejs
} // namespace HawkTracer

