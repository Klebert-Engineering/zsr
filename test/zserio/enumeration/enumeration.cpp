#include "base.hpp"

namespace {

PKG;

TEST(EnumTest, check_enum_items)
{
    auto* meta_enumeration = zsr::find<zsr::Enumeration>(pkg, "Enum");

    ASSERT_TRUE(meta_enumeration);
    ASSERT_EQ(meta_enumeration->items.size(), 3);

    auto ASSERT_ITEM = [&](auto ident, auto value) {
        auto* meta_item = zsr::find<zsr::EnumerationItem>(
            meta_enumeration, ident);

        ASSERT_TRUE(meta_item);
        ASSERT_VARIANT_EQ(meta_item->value, value);
    };

    ASSERT_ITEM("A", (unsigned)0);
    ASSERT_ITEM("B", (unsigned)1);
    ASSERT_ITEM("C", (unsigned)0xff);
}

}
