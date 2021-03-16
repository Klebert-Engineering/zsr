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

TEST(FindTest, recursive)
{
    auto resolved_b_a = zsr::find<zsr::Field>(zsr::packages(), "find_test.B_struct.a");
    ASSERT_TRUE(resolved_b_a);
    ASSERT_EQ(resolved_b_a->ident, "a");

    auto unresolved_nested_service_method = zsr::find<zsr::ServiceMethod>(
        zsr::packages(),
        "find_test.nested_schema.child");
    ASSERT_FALSE(unresolved_nested_service_method);

    auto resolved_nested_service_method = zsr::find<zsr::ServiceMethod>(
        zsr::packages(),
        "nested_schema.child.X_service.serviceMethod");
    ASSERT_TRUE(resolved_nested_service_method);

    auto resolved_nested_service_method_single_arg = zsr::find<zsr::ServiceMethod>(
        "nested_schema.child.X_service.serviceMethod");
    ASSERT_TRUE(resolved_nested_service_method_single_arg);
}

TEST(FindTest, recursive_fail)
{
    /* Path is empty */ {
        auto resolved = zsr::find<zsr::Field>(zsr::packages(), "");
        ASSERT_FALSE(resolved);
    }

    /* Package name is missing */ {
        auto resolved = zsr::find<zsr::Field>(zsr::packages(), "B_struct.a");
        ASSERT_FALSE(resolved);
    }
}

}
