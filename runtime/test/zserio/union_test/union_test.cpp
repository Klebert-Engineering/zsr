#include "base.hpp"

namespace {

PKG;

TEST_CASE("count_fields", "[UnionTest::count_fields]")
{
    auto meta_union_a = zsr::find<zsr::Compound>(pkg, "Union_a");

    REQUIRE(meta_union_a->type == zsr::Compound::Type::Union);
    REQUIRE(meta_union_a->fields.size() == 2u);
}

} // namespace
