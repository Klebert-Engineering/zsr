#include "base.hpp"

namespace {

PKG;

TEST(ConstantTest, check_enum_items)
{
    auto ASSERT_VALUE = [&](auto ident, auto value) {
        auto* meta_constant = zsr::find<zsr::Constant>(pkg, ident);

        ASSERT_TRUE(meta_constant);
        ASSERT_VARIANT_EQ(meta_constant->value, value);
    };

    ASSERT_VALUE("A", (uint32_t)1);
    ASSERT_VALUE("B", (int32_t)2);
    ASSERT_VALUE("C", (float)3);
    ASSERT_VALUE("D", (std::string)"Test");
    ASSERT_VALUE("E", (uint8_t)0b101);
}

}
