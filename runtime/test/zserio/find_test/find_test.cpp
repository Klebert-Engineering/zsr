#include "base.hpp"

#include <zsr/find.hpp>

namespace {

PKG;

TEST(FindTest, resolve_type)
{

    auto meta_struct_a = zsr::find<zsr::Compound>(pkg, "A_struct");
    ASSERT_TRUE(meta_struct_a);

    auto meta_struct_b = zsr::find<zsr::Compound>(pkg, "B_struct");
    ASSERT_TRUE(meta_struct_b);

    auto meta_field_b_a = zsr::find<zsr::Field>(*meta_struct_b, "a");
    ASSERT_TRUE(meta_field_b_a);

    auto resolved_b_a = zsr::resolveType<zsr::Compound>(zsr::packages(),
                                                        *meta_field_b_a->type);
    ASSERT_EQ(meta_struct_a, resolved_b_a);
}

}
