#include <hawktracer/parser/klass_register.hpp>
#include <hawktracer/parser/event.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

constexpr HT_EventKlassId invalid_klass_id = static_cast<HT_EventKlassId>(-1);

// TODO: do we need it in lib?
static char* ht_strdup(const char* str)
{
    size_t len = strlen(str);
    char* new_string = (char*)malloc(len + 1);
    strcpy(new_string, str);
    return new_string;
}

static Event create_klass_event(KlassRegister* klass_register, HT_EventKlassId klass_id, const char* klass_name, uint8_t field_count)
{
    auto event_klass_info = klass_register->get_klass(to_underlying(WellKnownKlasses::EventKlassInfoEventKlass));
    Event klass_event(event_klass_info);

    klass_event.set_value(event_klass_info->get_field("info_klass_id", true).get(), klass_id);
    klass_event.set_value(event_klass_info->get_field("event_klass_name", true).get(), ht_strdup(klass_name));
    klass_event.set_value(event_klass_info->get_field("field_count", true).get(), field_count);

    return klass_event;
}

static Event create_field_event(
        KlassRegister* klass_register, HT_EventKlassId klass_id, const char* field_type,
        const char* field_name, uint64_t type_size, MKCREFLECT_Types data_type)
{
    auto event_field_info = klass_register->get_klass(to_underlying(WellKnownKlasses::EventKlassFieldInfoEventKlass));
    Event field_event(event_field_info);

    field_event.set_value(event_field_info->get_field("info_klass_id", true).get(), klass_id);
    field_event.set_value(event_field_info->get_field("field_type", true).get(), ht_strdup(field_type));
    field_event.set_value(event_field_info->get_field("field_name", true).get(), ht_strdup(field_name));
    field_event.set_value(event_field_info->get_field("size", true).get(), type_size);
    field_event.set_value(event_field_info->get_field("data_type", true).get(), static_cast<uint8_t>(data_type));

    return field_event;
}

TEST(TestKlassRegister, ConstructorShouldCreateAllWellKnownTypes)
{
    // Arrange

    // Act
    KlassRegister klass_register;

    // Assert
    ASSERT_EQ(3u, klass_register.get_klasses().size());
    ASSERT_TRUE(klass_register.get_klass(to_underlying(WellKnownKlasses::EventKlass)));
    ASSERT_TRUE(klass_register.get_klass(to_underlying(WellKnownKlasses::EventKlassInfoEventKlass)));
    ASSERT_TRUE(klass_register.get_klass(to_underlying(WellKnownKlasses::EventKlassFieldInfoEventKlass)));
}

TEST(TestKlassRegister, KlassExistsShouldReturnFalseIfKlassDoesNotExist)
{
    // Arrange
    KlassRegister klass_register;

    // Act & Assert
    ASSERT_FALSE(klass_register.klass_exists(invalid_klass_id));
}

TEST(TestKlassRegister, KlassExistsShouldReturnTrueIfKlassExists)
{
    // Arrange
    KlassRegister klass_register;

    // Act & Assert
    ASSERT_TRUE(klass_register.klass_exists(to_underlying(WellKnownKlasses::EventKlassInfoEventKlass)));
}

TEST(TestKlassRegister, GetKlassIdShouldReturnZeroIfKlassDoesNotExist)
{
    // Arrange
    KlassRegister klass_register;

    // Act & Assert
    ASSERT_EQ(0u, klass_register.get_klass_id("non-existing-klass"));
}

TEST(TestKlassRegister, GetKlassIdShouldReturnCorrectIdIfKlassExists)
{
    // Arrange
    KlassRegister klass_register;

    // Act & Assert
    ASSERT_EQ(to_underlying(WellKnownKlasses::EventKlass), klass_register.get_klass_id("HT_Event"));
}

TEST(TestKlassRegister, GetKlassShouldReturnEmptyPointerIfKlassDoesNotExist)
{
    // Arrange
    KlassRegister klass_register;

    // Act & Assert
    ASSERT_FALSE(klass_register.get_klass("non-existing-klass"));
}

TEST(TestKlassRegister, GetKlassShouldReturnValidKlassIfKlassExists)
{
    // Arrange
    KlassRegister klass_register;
    const char* klass_name = "HT_Event";

    // Act
    auto klass = klass_register.get_klass(klass_name);

    // Assert
    ASSERT_TRUE(klass);
    ASSERT_STREQ(klass_name, klass->get_name().c_str());
}

TEST(TestKlassRegister, RegisterShouldSilentlySkipHandlingNonRegisterEvents)
{
    // Arrange
    KlassRegister klass_register;
    auto non_register_klass = std::make_shared<EventKlass>("new-name", 320);
    Event non_register_event(non_register_klass);

    // Act & Assert
    // We don't expect crash here
    klass_register.handle_register_events(non_register_event);
    ASSERT_EQ(3u, klass_register.get_klasses().size());
}

TEST(TestKlassRegister, KlassEventShouldCreateNewKlassInRegister)
{
    // Arrange
    KlassRegister klass_register;
    const uint32_t new_klass_id = 20u;
    const char* new_klass_name = "new_klass";
    Event klass_event = create_klass_event(&klass_register, new_klass_id, new_klass_name, 3u);

    // Act
    klass_register.handle_register_events(klass_event);

    // Assert
    auto klass = klass_register.get_klass(new_klass_id);
    ASSERT_TRUE(klass);
    ASSERT_STREQ(new_klass_name, klass->get_name().c_str());
}

TEST(TestKlassRegister, SecondKlassWithTheSameIdShouldNotBeAddedToRegistry)
{
    // Arrange
    KlassRegister klass_register;
    const uint32_t new_klass_id = 20u;
    const char* new_klass_name1 = "new_klass1";
    const char* new_klass_name2 = "new_klass2";
    Event klass_event1 = create_klass_event(&klass_register, new_klass_id, new_klass_name1, 3u);
    Event klass_event2 = create_klass_event(&klass_register, new_klass_id, new_klass_name2, 3u);
    klass_register.handle_register_events(klass_event1);

    // Act
    klass_register.handle_register_events(klass_event2);

    // Assert
    ASSERT_EQ(4u, klass_register.get_klasses().size());
    auto klass = klass_register.get_klass(new_klass_id);
    ASSERT_TRUE(klass);
    ASSERT_STREQ(new_klass_name1, klass->get_name().c_str());
}

TEST(TestKlassRegister, FieldEventShouldAddFieldToKlassInRegistry)
{
    // Arrange
    KlassRegister klass_register;
    const uint32_t new_klass_id = 20u;
    const char* field_name = "field-name";
    Event klass_event = create_klass_event(&klass_register, new_klass_id, "new_klass", 3u);
    klass_register.handle_register_events(klass_event);
    Event field_event = create_field_event(&klass_register, new_klass_id, "field_type", field_name, 4, MKCREFLECT_TYPES_INTEGER);

    // Act
    klass_register.handle_register_events(field_event);

    // Assert
    auto klass = klass_register.get_klass(new_klass_id);
    auto field = klass->get_field(field_name, false);
    ASSERT_TRUE(field);
    ASSERT_STREQ(field_name, field->get_name().c_str());
}

TEST(TestKlassRegister, SecondFieldEventWithTheSameNameShouldNotAddFieldToKlassInRegistry)
{
    // Arrange
    KlassRegister klass_register;
    const uint32_t new_klass_id = 20u;
    const char* field_name = "field-name";
    const char* field_type1 = "field-type1";
    const char* field_type2 = "field-type2";
    Event klass_event = create_klass_event(&klass_register, new_klass_id, "new_klass", 3u);
    klass_register.handle_register_events(klass_event);
    Event field_event1 = create_field_event(&klass_register, new_klass_id, field_type1, field_name, 4, MKCREFLECT_TYPES_INTEGER);
    Event field_event2 = create_field_event(&klass_register, new_klass_id, field_type2, field_name, 4, MKCREFLECT_TYPES_INTEGER);
    klass_register.handle_register_events(field_event1);

    // Act
    klass_register.handle_register_events(field_event2);

    // Assert
    auto klass = klass_register.get_klass(new_klass_id);
    auto field = klass->get_field(field_name, false);
    ASSERT_TRUE(field);
    ASSERT_STREQ(field_type1, field->get_type_name().c_str());
}

TEST(TestKlassRegister, StructFieldEventShouldSetKlassToField)
{
    // Arrange
    KlassRegister klass_register;
    const uint32_t new_klass_id = 20u;
    const char* field_name = "field-name";
    const char* type_name = "HT_Event";
    Event klass_event = create_klass_event(&klass_register, new_klass_id, "new_klass", 3u);
    klass_register.handle_register_events(klass_event);
    Event field_event = create_field_event(&klass_register, new_klass_id, type_name, field_name, 4, MKCREFLECT_TYPES_STRUCT);

    // Act
    klass_register.handle_register_events(field_event);

    // Assert
    auto klass = klass_register.get_klass(new_klass_id);
    auto field = klass->get_field(field_name, false);
    ASSERT_TRUE(field);
    ASSERT_TRUE(field->get_klass());
    ASSERT_STREQ(type_name, field->get_klass()->get_name().c_str());
}

TEST(TestKlassRegister, FieldEventShouldNotUpdateWellKnownKlasses)
{
    // Arrange
    KlassRegister klass_register;
    const char* field_name = "test-name";
    auto base_klass = klass_register.get_klass(to_underlying(WellKnownKlasses::EventKlass));
    Event field_event = create_field_event(&klass_register, base_klass->get_id(), "type", field_name, 4, MKCREFLECT_TYPES_INTEGER);

    // Act
    klass_register.handle_register_events(field_event);

    // Assert
    ASSERT_FALSE(base_klass->get_field(field_name, false));
}
