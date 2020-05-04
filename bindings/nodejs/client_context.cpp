#include "client_context.hpp"

#include "hawktracer/client_utils/stream_factory.hpp"
#include "hawktracer/parser/make_unique.hpp"
#include <iostream>
#include <utility>

namespace HawkTracer
{
namespace Nodejs
{

std::unique_ptr<ClientContext>
ClientContext::create(const std::string& source, const std::string& map_files, EventCallback event_callback)
{
    std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source);
    if (!stream) {
        return nullptr;
    }

    auto klass_register = parser::make_unique<parser::KlassRegister>();
    auto reader = parser::make_unique<parser::ProtocolReader>(klass_register.get(), std::move(stream), true);
    auto converter = parser::make_unique<ClientUtils::Converter>();
    converter->set_tracepoint_map(map_files);

    std::unique_ptr<ClientContext> context{new ClientContext(std::move(reader),
                                                             std::move(klass_register),
                                                             std::move(converter),
                                                             std::move(event_callback))};
    return context;
}

ClientContext::ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                             std::unique_ptr<parser::KlassRegister> klass_register,
                             std::unique_ptr<ClientUtils::Converter> converter,
                             EventCallback event_callback)
    : _klass_register(std::move(klass_register)), _reader(std::move(reader)), _label_mapper(std::move(converter)),
      _event_callback(std::move(event_callback))
{
    _reader->register_events_listener(
        [this](const parser::Event& event)
        {
            {
                std::lock_guard<std::mutex> lock{_buffer_mutex};
                _buffer.emplace_back(event, _label_mapper->get_label(event));  // Event is copied once.
            }
            _event_callback();
        });

    _reader->start();
}

ClientContext::~ClientContext()
{
    _reader->stop();

    if (!_buffer.empty()) {
        std::cerr << _buffer.size() << " events were not processed." << std::endl;
    }
}

// This method can be called from any thread, while the event listener is called from reader thread.
std::vector<LabeledEvent> ClientContext::take_events()
{
    std::lock_guard<std::mutex> lock{_buffer_mutex};
    std::vector<LabeledEvent> new_buffer;
    std::swap(new_buffer, _buffer);
    return new_buffer;
}

} // namespace Nodejs
} // namespace HawkTracer

