#pragma once

#include "zsr/types.hpp"

namespace zsr {
namespace impl {

template <class>
struct child_iter
{};

#define DECL_ITER(TYPE, PARENT_TYPE, LIST)                                     \
    template <>                                                                \
    struct child_iter<TYPE>                                                    \
    {                                                                          \
        static auto get(const PARENT_TYPE* r)                                  \
        {                                                                      \
            return std::make_pair(r->LIST.cbegin(), r->LIST.cend());           \
        }                                                                      \
    };

/* clang-format off */
/*        Target           Source       Member */
DECL_ITER(Enumeration,     Package,     enumerations)
DECL_ITER(EnumerationItem, Enumeration, items)

DECL_ITER(Bitmask,         Package,     bitmasks)
DECL_ITER(BitmaskValue,    Bitmask,     values)

DECL_ITER(Constant,        Package,     constants)

DECL_ITER(Compound,        Package,     compounds)
DECL_ITER(Field,           Compound,    fields)
DECL_ITER(Parameter,       Compound,    parameters)
DECL_ITER(Function,        Compound,    functions)
DECL_ITER(ChoiceCase,      Compound,    cases)
/* clang-format on */

#undef DECL_ITER

} // namespace impl

/**
 * Utility functions for searching metadata.
 *
 * Example:
 *   auto field = zsr::find<zsr::Field>(compound, "name")
 */
template <class _Type, class _Root>
const _Type* find(const _Root* r, const std::string& ident)
{
    auto [begin, end] = impl::child_iter<_Type>::get(r);
    auto iter = std::find_if(begin, end, [&](const auto* c) {
        return c->ident == ident;
    });
    if (iter != end)
        return *iter;
    return nullptr;
}

} // namespace zsr
