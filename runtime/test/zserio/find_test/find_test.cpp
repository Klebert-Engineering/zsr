#include "base.hpp"

#include <zsr/find.hpp>

namespace {

PKG;

TEST_CASE("resolve_type", "[FindTest::resolve_type]")
{
    auto meta_struct_a = zsr::find<zsr::Compound>(pkg, "A_struct");
    REQUIRE(meta_struct_a);

    auto meta_struct_b = zsr::find<zsr::Compound>(pkg, "B_struct");
    REQUIRE(meta_struct_b);

    auto meta_field_b_a = zsr::find<zsr::Field>(*meta_struct_b, "a");
    REQUIRE(meta_field_b_a);

    auto resolved_b_a = zsr::resolveType<zsr::Compound>(zsr::packages(),
                                                        *meta_field_b_a->type);
    REQUIRE(meta_struct_a == resolved_b_a);
}

TEST_CASE("recursive", "[FindTest::recursive]")
{
    auto resolved_b_a = zsr::find<zsr::Field>(zsr::packages(), "zsr.find_test.B_struct.a");
    REQUIRE(resolved_b_a);
    REQUIRE(resolved_b_a->ident == "a");

    auto unresolved_nested_service_method = zsr::find<zsr::ServiceMethod>(
        zsr::packages(),
        "zsr.find_test.nested_schema.child");
    REQUIRE(!unresolved_nested_service_method);

    auto resolved_nested_service_method = zsr::find<zsr::ServiceMethod>(
        zsr::packages(),
        "zsr.nested_schema.child.X_service.serviceMethod");
    REQUIRE(resolved_nested_service_method);

    auto resolved_nested_service_method_single_arg = zsr::find<zsr::ServiceMethod>(
        "zsr.nested_schema.child.X_service.serviceMethod");
    REQUIRE(resolved_nested_service_method_single_arg);
}

TEST_CASE("recursive_fail", "[FindTest::recursive_fail]")
{
    /* Path is empty */ {
        auto resolved = zsr::find<zsr::Field>(zsr::packages(), "");
        REQUIRE(!resolved);
    }

    /* Package name is missing */ {
        auto resolved = zsr::find<zsr::Field>(zsr::packages(), "B_struct.a");
        REQUIRE(!resolved);
    }
}

}
