#ifndef HAWKTRACER_PARSER_KLASS_REGISTER_HPP
#define HAWKTRACER_PARSER_KLASS_REGISTER_HPP

#include <hawktracer/base_types.h>
#include <hawktracer/parser/event_klass.hpp>

#include <mutex>
#include <unordered_map>

namespace HawkTracer {
namespace parser {

enum class WellKnownKlasses : HT_EventKlassId
{
    EventKlass = 1,
    EventKlassInfoEventKlass = 2,
    EventKlassFieldInfoEventKlass = 3
};

class KlassRegister
{
public:
    KlassRegister();

    KlassRegister(const KlassRegister&) = delete;
    KlassRegister& operator=(const KlassRegister&) = delete;

    static bool is_well_known_klass(HT_EventKlassId klass_id);

    void handle_register_events(const Event& event);

    std::shared_ptr<const EventKlass> get_klass(HT_EventKlassId klass_id) const;
    std::shared_ptr<const EventKlass> get_klass(const std::string& name) const;
    HT_EventKlassId get_klass_id(const std::string& name) const;

    bool klass_exists(HT_EventKlassId klass_id) const;
    std::unordered_map<HT_EventKlassId, std::shared_ptr<EventKlass> > get_klasses() const;

private:
    void _add_klass(std::unique_ptr<EventKlass> klass);
    void _add_klass_field(HT_EventKlassId klass_id, std::unique_ptr<EventKlassField> field);

    using lock_guard = std::lock_guard<std::mutex>;

    mutable std::mutex _register_mtx;
    std::unordered_map<HT_EventKlassId, std::shared_ptr<EventKlass>> _register;
};

} // namespace parser
} // namespace HawkTracer

#endif // HAWKTRACER_PARSER_KLASS_REGISTER_HPP
