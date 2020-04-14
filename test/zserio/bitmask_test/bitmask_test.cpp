#include "base.hpp"

namespace {

PKG;

TEST(BitmaskTest, check_bitmask_values) {
    auto* meta_bitmask = zsr::find<zsr::Bitmask>(pkg, "Mask");

    ASSERT_TRUE(meta_bitmask);
    ASSERT_EQ(meta_bitmask->values.size(), 3);

    auto ASSERT_VALUE = [&](auto ident, unsigned val) {
        auto* meta_value = zsr::find<zsr::BitmaskValue>(
            meta_bitmask, ident);

        ASSERT_TRUE(meta_value);
        ASSERT_VARIANT_EQ(meta_value->value, val);
    };

    ASSERT_VALUE("A", 1);
    ASSERT_VALUE("B", 2);
    ASSERT_VALUE("C", 0xff);
}

}
