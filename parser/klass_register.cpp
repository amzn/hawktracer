#include "klass_register.hpp"
#include "event.hpp"
#include "make_unique.hpp"

namespace HawkTracer {
namespace parser {

KlassRegister& KlassRegister::get()
{
    static KlassRegister klass_register;
    return klass_register;
}

KlassRegister::KlassRegister()
{
    EventKlass base_event("HT_Event", to_underlying(WellKnownKlasses::EventKlass));
    base_event.add_field(make_unique<EventKlassField>("type", "uint32_t", FieldTypeId::UINT32));
    base_event.add_field(make_unique<EventKlassField>("timestamp", "uint64_t", FieldTypeId::UINT64));
    base_event.add_field(make_unique<EventKlassField>("id", "uint64_t", FieldTypeId::UINT64));
    add_klass(std::move(base_event));

    EventKlass klass_info_event("HT_EventKlassInfoEvent", to_underlying(WellKnownKlasses::EventKlassInfoEventKlass));
    klass_info_event.add_field(make_unique<EventKlassField>("event_type", "uint32_t", FieldTypeId::UINT32));
    klass_info_event.add_field(make_unique<EventKlassField>("event_klass_name", "const char*", FieldTypeId::STRING));
    klass_info_event.add_field(make_unique<EventKlassField>("field_count", "uint8_t", FieldTypeId::UINT8));
    add_klass(std::move(klass_info_event));

    EventKlass klass_field_info_event("HT_EventKlassFieldInfoEvent", to_underlying(WellKnownKlasses::EventKlassFieldInfoEventKlass));
    klass_field_info_event.add_field(make_unique<EventKlassField>("event_type", "uint32_t", FieldTypeId::UINT32));
    klass_field_info_event.add_field(make_unique<EventKlassField>("field_type", "const char*", FieldTypeId::STRING));
    klass_field_info_event.add_field(make_unique<EventKlassField>("field_name", "const char*", FieldTypeId::STRING));
    klass_field_info_event.add_field(make_unique<EventKlassField>("size", "uint64_t", FieldTypeId::UINT64));
    klass_field_info_event.add_field(make_unique<EventKlassField>("data_type", "uint8_t", FieldTypeId::UINT8));
    add_klass(std::move(klass_field_info_event));
}

bool KlassRegister::is_well_known_klass(uint32_t klass_id)
{
    switch ((WellKnownKlasses)klass_id)
    {
    case WellKnownKlasses::EventKlass:
    case WellKnownKlasses::EventKlassInfoEventKlass:
    case WellKnownKlasses::EventKlassFieldInfoEventKlass:
        return true;
    default:
        return false;
    }
}

void KlassRegister::handle_register_events(const Event& event)
{
    switch (static_cast<WellKnownKlasses>(event.get_klass().get_id()))
    {
    case WellKnownKlasses::EventKlassInfoEventKlass:
    {
        uint32_t klass_id = event.get_value<uint32_t>("event_type");
        if (!KlassRegister::get().klass_exists(klass_id))
        {
            KlassRegister::get().add_klass({event.get_value<char*>("event_klass_name"), klass_id});
        }
        break;
    }
    case WellKnownKlasses::EventKlassFieldInfoEventKlass:
    {
        auto klass_id = event.get_value<uint32_t>("event_type");
        if (!KlassRegister::is_well_known_klass(klass_id))
        {
            auto field = make_unique<EventKlassField>(event.get_value<char*>("field_name"),
                    event.get_value<char*>("field_type"),
                    get_type_id(event.get_value<uint64_t>("size"), static_cast<MKCREFLECT_Types>(event.get_value<uint8_t>("data_type"))));

            KlassRegister::get().add_klass_field(klass_id, std::move(field));
        }
        break;
    }
    default:
        break;
    }
}

const EventKlass& KlassRegister::get_klass(uint32_t klass_id) const
{
    return _register.find(klass_id)->second;
}

uint32_t KlassRegister::get_klass_id(const std::string& name) const
{
    for (const auto& klass : _register)
    {
        if (klass.second.get_name() == name)
        {
            return klass.second.get_id();
        }
    }

    return 0;
}

void KlassRegister::add_klass(EventKlass klass)
{
    if (_register.find(klass.get_id()) == _register.end())
    {
        _register.insert(std::make_pair(klass.get_id(), std::move(klass)));
    }
}

void KlassRegister::add_klass_field(uint32_t klass_id, std::unique_ptr<EventKlassField> field)
{
    _register.at(klass_id).add_field(std::move(field));
}

} // namespace parser
} // namespace HawkTracer
