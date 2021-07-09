#include "base.hpp"

namespace {

PKG;

TEST_CASE("check_enum_items", "[EnumTest::check_enum_items]")
{
    auto* meta_enumeration = zsr::find<zsr::Enumeration>(pkg, "Enum");

    REQUIRE(meta_enumeration);
    REQUIRE(&meta_enumeration->parent == &pkg);
    REQUIRE(meta_enumeration->items.size() == 3);

    auto ASSERT_ITEM = [&](auto ident, auto value) {
        auto* meta_item = zsr::find<zsr::EnumerationItem>(
            *meta_enumeration, ident);

        REQUIRE(meta_item);
        REQUIRE(&meta_item->parent == meta_enumeration);
        REQUIRE(meta_item->value == value);
    };

    ASSERT_ITEM("A", (unsigned)0);
    ASSERT_ITEM("B", (unsigned)1);
    ASSERT_ITEM("C", (unsigned)0xff);
}

TEST_CASE("enum_member", "[EnumTest::enum_member]") {
    auto* meta_struct = zsr::find<zsr::Compound>(pkg, "Owner");
    REQUIRE(meta_struct);

    auto* meta_field = zsr::find<zsr::Field>(*meta_struct, "a");
    REQUIRE(meta_field);

    auto type = meta_field->type;
    REQUIRE(type);
    REQUIRE(type->ctype.type == zsr::CType::UInt);
    REQUIRE(!type->ctype.array);
}


}
