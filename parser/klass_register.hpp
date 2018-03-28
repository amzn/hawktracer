#ifndef HAWKTRACER_PARSER_KLASS_REGISTER_HPP
#define HAWKTRACER_PARSER_KLASS_REGISTER_HPP

#include "parser/event_klass.hpp"

#include <unordered_map>

namespace HawkTracer {
namespace parser {

enum class WellKnownKlasses : uint32_t
{
    EventKlass = 1,
    EventKlassInfoEventKlass = 2,
    EventKlassFieldInfoEventKlass = 3
};

class KlassRegister
{
public:
    static KlassRegister& get();

    KlassRegister(const KlassRegister&) = delete;
    KlassRegister& operator=(const KlassRegister&) = delete;

    static bool is_well_known_klass(uint32_t klass_id);

    static void handle_register_events(const Event& event);

    const EventKlass& get_klass(uint32_t klass_id) const;
    const EventKlass& get_klass(const std::string& name) const;
    uint32_t get_klass_id(const std::string& name) const;
    void add_klass(EventKlass klass);
    void add_klass_field(uint32_t klass_id, std::unique_ptr<EventKlassField> field);
    bool klass_exists(uint32_t klass_id) const { return _register.find(klass_id) != _register.end(); }
    const std::unordered_map<uint32_t, EventKlass>& get_klasses() const { return _register; }

private:
    KlassRegister();

    std::unordered_map<uint32_t, EventKlass> _register;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_KLASS_REGISTER_HPP
