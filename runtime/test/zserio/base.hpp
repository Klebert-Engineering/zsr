#pragma once

#include "gtest/gtest.h"

#include "zsr/variant.hpp"
#include "zsr/types.hpp"
#include "zsr/find.hpp"

#include "reflection-main.hpp"

#define PKG                                     \
    static const auto& pkg = zsr::packages().front();

template <class _Type>
::testing::AssertionResult AssertVariantEq(const char* expr1,
                                           const char* expr2,
                                           const zsr::Variant& val1,
                                           const _Type& val2)
{
    auto opt = val1.get<_Type>();
    if (!opt)
        return ::testing::AssertionFailure()
            << expr1 << " does not hold value of type " << expr2;

    if (*opt != val2)
        return ::testing::AssertionFailure()
            << expr1 << " (" << *opt << ") is not equal to "
            << expr2 << " (" << val2 << ")";

    return ::testing::AssertionSuccess();
}

#define ASSERT_VARIANT_EQ(a, b)                 \
    EXPECT_PRED_FORMAT2(AssertVariantEq, a, b)
