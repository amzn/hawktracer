#include "protocol_reader.hpp"
#include "klass_register.hpp"
#include "event.hpp"

#include <cassert>
#include <cstring>

#define FLAT_EVENT_STRUCTURE

namespace HawkTracer
{
namespace parser
{

ProtocolReader::ProtocolReader(std::unique_ptr<Stream> stream) :
    _stream(std::move(stream))
{
    register_events_listener(KlassRegister::handle_register_events);
}

ProtocolReader::~ProtocolReader()
{
    stop();
}

void ProtocolReader::register_events_listener(OnNewEventCallback callback)
{
    _on_new_event_callbacks.push_back(std::move(callback));
}

bool ProtocolReader::start()
{
    if (_stream->start())
    {
        _is_running = true;
        _thread = std::thread([this] { _read_events(); });
        return true;
    }

    return false;
}

void ProtocolReader::stop()
{
    _is_running = false;
    if (_thread.joinable())
    {
        _thread.join();
    }
    _stream->stop();
}

void ProtocolReader::wait_for_complete()
{
    std::unique_lock l(_mtx_cv);
    _cv.wait(l, [this] { return !_is_running; });
}

void ProtocolReader::_read_events()
{
    while (_is_running)
    {
        bool is_error = false;
        Event base_event(KlassRegister::get().get_klass(to_underlying(WellKnownKlasses::EventKlass)));
        _read_event(is_error, base_event, nullptr);

        if (is_error)
        {
            break;
        }

        auto klass_id = base_event.get_value<uint32_t>("type");

        if (klass_id == to_underlying(WellKnownKlasses::EventKlass))
        {
            _call_callbacks(base_event);
            continue;
        }

        Event event(KlassRegister::get().get_klass(base_event.get_value<uint32_t>("type")));
#ifdef FLAT_EVENT_STRUCTURE
        _read_event(is_error, event, nullptr);
        event.merge(std::move(base_event));
#else
        _read_event(is_error, event, &base_event);
#endif // FLAT_EVENT_STRUCTURE

        if (is_error)
        {
            break;
        }

        _call_callbacks(event);
    }

    _is_running = false;
    _cv.notify_one();
}

void ProtocolReader::_read_event(bool& is_error, Event& event, Event* base_event)
{
    for (const auto& field : event.get_klass().get_fields())
    {
        FieldType value;
        if (((field->is_numeric() || field->get_type_id() == FieldTypeId::POINTER) && !_read_numeric(value, *field)) || // read numeric or pointer
                (field->get_type_id() == FieldTypeId::STRING && !_read_string(value)) || // read string
                (field->get_type_id() == FieldTypeId::STRUCT && !_read_struct(value, *field, &event, base_event))) // read struct
        {
            is_error = true;
            return;
        }

#ifdef FLAT_EVENT_STRUCTURE
        if (field->get_type_id() != FieldTypeId::STRUCT)
        {
            event.set_value(field.get(), value);
        }
#else
        event.set_value(field.get(), value);
#endif // FLAT_EVENT_STRUCTURE
    }

    is_error = false;
}

bool ProtocolReader::_read_string(FieldType& value)
{
    size_t length = 32;
    size_t pos = 0;
    char* data = (char*)malloc(length);
    int c;
    while ((c = _stream->read_byte()) > 0)
    {
        data[pos++] = (char)c;
        if (pos == length)
        {
            length *= 2;
            data = (char*)realloc(data, length);
        }
    }
    if (c < 0)
    {
        free(data);
        return false;
    }

    data[pos] = 0;
    value.f_STRING = data;
    return true;
}

bool ProtocolReader::_read_numeric(FieldType& value, const EventKlassField& field)
{
    char buff[16];
    if (!_stream->read_data(buff, field.get_sizeof()))
    {
        return false;
    }

    switch (field.get_type_id())
    {
#define SET_VALUE(field_id, c_type) case FieldTypeId::field_id: value.f_##field_id = *((c_type*)buff); break
    SET_VALUE(UINT8, uint8_t);
    SET_VALUE(INT8, int8_t);
    SET_VALUE(UINT16, uint16_t);
    SET_VALUE(INT16, int16_t);
    SET_VALUE(UINT32, uint32_t);
    SET_VALUE(INT32, int32_t);
    SET_VALUE(UINT64, uint64_t);
    SET_VALUE(INT64, int64_t);
    SET_VALUE(POINTER, void*);
#undef SET_VALUE
    default: assert(0); return false;
    }

    return true;
}

bool ProtocolReader::_read_struct(FieldType& value, const EventKlassField& field, Event* event, Event* base_event)
{
    if (field.get_type_name() == "HT_Event" && field.get_name() == "base")
    {
#ifndef FLAT_EVENT_STRUCTURE
        assert(base_event != nullptr);
        value.f_EVENT = new Event(std::move(*base_event));
#endif // FLAT_EVENT_STRUCTURE
        return true;
    }
    else
    {
        bool is_error;
        Event sub_event(KlassRegister::get().get_klass(KlassRegister::get().get_klass_id(field.get_type_name())));
        _read_event(is_error, sub_event, base_event);
#ifdef FLAT_EVENT_STRUCTURE
        event->merge(std::move(sub_event));
#else
        value.f_EVENT = new Event(std::move(sub_event));
#endif // FLAT_EVENT_STRUCTURE
        return !is_error;
    }
}

void ProtocolReader::_call_callbacks(const Event& event)
{
    for (const auto& callback : _on_new_event_callbacks)
    {
        callback(event);
    }
}

} // namespace parser
} // namespace HawkTracer
