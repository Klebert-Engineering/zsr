#include "base.hpp"

namespace {

PKG;

TEST(UnionTest, count_fields)
{
    auto meta_union_a = zsr::find<zsr::Compound>(pkg, "union_a");

    ASSERT_EQ(meta_union_a->fields.size(), 2u);
}

} // namespace
