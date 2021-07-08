#include "base.hpp"

namespace {

PKG;

TEST_CASE("count_choices", "[ChoiceTest::count_choices]")
{
    auto meta_choice_a = zsr::find<zsr::Compound>(pkg, "Choice_a");

    REQUIRE(meta_choice_a->type == zsr::Compound::Type::Choice);
}

} // namespace
