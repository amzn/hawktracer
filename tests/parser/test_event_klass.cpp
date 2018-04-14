#include <hawktracer/parser/event_klass.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include <gtest/gtest.h>

using namespace HawkTracer::parser;

TEST(TestEventKlass, ValidateAccessorMethods)
{
    // Arrange
    const std::string klass_name = "test_klass";
    EventKlass klass(klass_name, 1);

    // Act &  Assert
    ASSERT_EQ(1u, klass.get_id());
    ASSERT_STREQ(klass_name.c_str(), klass.get_name().c_str());
}

TEST(TestEventKlass, AddFieldShouldNotAddTheSameFieldTwice)
{
    // Arrange
    EventKlass klass("test_klass", 1);
    const std::string field_name = "field1";
    const std::string type_name = "field_type";
    auto first_field = make_unique<EventKlassField>(field_name, type_name, FieldTypeId::INT8);
    klass.add_field(std::move(first_field));
    auto second_field = make_unique<EventKlassField>(field_name, "field_type_2", FieldTypeId::INT8);

    // Act
    klass.add_field(std::move(second_field));

    // Assert
    ASSERT_EQ(1u, klass.get_fields().size());
    ASSERT_STREQ(type_name.c_str(), klass.get_fields().front()->get_type_name().c_str());
}

TEST(TestEventKlass, GetFieldShouldReturnEmptyPointerIfFieldDoesNotExist_NonRecursive)
{
    // Arrange
    EventKlass klass("test_klass", 1);

    // Act
    auto field = klass.get_field("non_existing_name", false);

    // Assert
    ASSERT_FALSE(field);
}

TEST(TestEventKlass, GetFieldShouldReturnEmptyPointerIfFieldExistInSubKlass_NonRecursive)
{
    // Arrange
    const std::string base_field_name = "base_field_name";
    auto base_klass = std::make_shared<EventKlass>("base_klass", 0);
    base_klass->add_field(make_unique<EventKlassField>(base_field_name.c_str(), "type", FieldTypeId::INT8));
    EventKlass klass("test_klass", 1);
    klass.add_field(make_unique<EventKlassField>("parent", "type", FieldTypeId::INT8, base_klass));

    // Act
    auto field = klass.get_field(base_field_name.c_str(), false);

    // Assert
    ASSERT_FALSE(field);
}

TEST(TestEventKlass, GetFieldShouldReturnFieldIfFieldExists_NonRecursive)
{
    // Arrange
    EventKlass klass("test_klass", 1);
    const std::string field_name = "test_field";
    klass.add_field(make_unique<EventKlassField>(field_name, "type", FieldTypeId::INT8));

    // Act
    auto field = klass.get_field(field_name.c_str(), false);

    // Assert
    ASSERT_TRUE(field);
    ASSERT_STREQ(field_name.c_str(), field->get_name().c_str());
}

TEST(TestEventKlass, GetFieldShouldReturnFieldIfFieldExistInSubKlass_Recursive)
{
    // Arrange
    const std::string base_field_name = "base_field_name";
    auto base_klass = std::make_shared<EventKlass>("base_klass", 0);
    base_klass->add_field(make_unique<EventKlassField>(base_field_name.c_str(), "type", FieldTypeId::INT8));
    EventKlass klass("test_klass", 1);
    klass.add_field(make_unique<EventKlassField>("base", "type", FieldTypeId::STRUCT, base_klass));

    // Act
    auto field = klass.get_field(base_field_name.c_str(), true);

    // Assert
    ASSERT_TRUE(field);
}

TEST(TestEventKlassField, ShouldReturnCorrectSizeBasedOnType)
{
    // Arrange
    auto make_field = [] (FieldTypeId type_id) {
        return EventKlassField("field_name", "type", type_id);
    };

    EventKlassField fields[] = {
        make_field(FieldTypeId::INT8),
        make_field(FieldTypeId::UINT8),
        make_field(FieldTypeId::INT16),
        make_field(FieldTypeId::UINT16),
        make_field(FieldTypeId::INT32),
        make_field(FieldTypeId::UINT32),
        make_field(FieldTypeId::INT64),
        make_field(FieldTypeId::UINT64)
    };
    size_t expected_sizes[] = {1, 1, 2, 2, 4, 4, 8, 8};

    // Act & Assert
    for (size_t i = 0; i < sizeof(fields)/sizeof(fields[0]); i++)
    {
        ASSERT_EQ(expected_sizes[i], fields[i].get_sizeof());
    }
}

TEST(TestEventKlass, GetTypeIdShouldReturnCorrectIntTypeBasedOnSizeAndDataType)
{
    // Arrange
    uint64_t input[] = {1, 2, 4, 8};
    FieldTypeId expected[] = {FieldTypeId::UINT8, FieldTypeId::UINT16, FieldTypeId::UINT32, FieldTypeId::UINT64};

    // Act & Assert
    for (size_t i = 0; i < sizeof(input)/sizeof(input[0]); i++)
    {
        ASSERT_EQ(expected[i], get_type_id(input[i], MKCREFLECT_TYPES_INTEGER));
    }
}

TEST(TestEventKlass, GetTypeIdShouldReturnPointerWhenDataTypeIsPointer)
{
    // Arrange

    // Act & Assert
    ASSERT_EQ(FieldTypeId::POINTER, get_type_id(23, MKCREFLECT_TYPES_POINTER));
    ASSERT_EQ(FieldTypeId::POINTER, get_type_id(0, MKCREFLECT_TYPES_POINTER));
}

TEST(TestEventKlass, GetTypeIdShouldReturnStringWhenDataTypeIsString)
{
    // Arrange

    // Act & Assert
    ASSERT_EQ(FieldTypeId::STRING, get_type_id(9999, MKCREFLECT_TYPES_STRING));
    ASSERT_EQ(FieldTypeId::STRING, get_type_id(0, MKCREFLECT_TYPES_STRING));
}
