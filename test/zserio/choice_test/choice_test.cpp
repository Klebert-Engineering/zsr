#include "base.hpp"

namespace {

PKG;

TEST(ChoiceTest, count_choices)
{
    auto meta_choice_a = zsr::find<zsr::Compound>(pkg, "choice_a");

    ASSERT_EQ(meta_choice_a->cases.size(), 3u);
}

} // namespace
