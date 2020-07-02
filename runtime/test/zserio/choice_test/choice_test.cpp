#include "base.hpp"

namespace {

PKG;

TEST(ChoiceTest, count_choices)
{
    auto meta_choice_a = zsr::find<zsr::Compound>(pkg, "Choice_a");

    ASSERT_EQ(meta_choice_a->type, zsr::Compound::Type::Choice);
}

} // namespace
