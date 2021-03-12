#pragma once

#include "zsr/types.hpp"
#include "stx/string.h"

namespace zsr {
namespace impl {

template <class>
struct ZSR_EXPORT child_iter
{};

template <>
struct ZSR_EXPORT child_iter<zsr::Package>
{
    using ParentType = std::deque<zsr::Package>;
    static auto get(const std::deque<zsr::Package>& r)
    {
        return std::make_pair(r.cbegin(), r.cend());
    }
};

#define DECL_ITER(TYPE, PARENT_TYPE, LIST)                                   \
    template <>                                                              \
    struct child_iter<TYPE>                                                  \
    {                                                                        \
        using ParentType = PARENT_TYPE;                                      \
        static auto get(const PARENT_TYPE& r)                                \
        {                                                                    \
            return std::make_pair(r.LIST.cbegin(), r.LIST.cend());           \
        }                                                                    \
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

DECL_ITER(SubType,         Package,     subTypes)

DECL_ITER(Service,         Package,     services)
DECL_ITER(ServiceMethod,   Service,     methods)
/* clang-format on */

#undef DECL_ITER

template <class _Type, class _Root>
const _Type* find(const _Root& r, const std::string_view& ident)
{
    auto [begin, end] = impl::child_iter<_Type>::get(r);
    auto iter = std::find_if(begin, end, [&](const auto& c) {
        return c.ident == ident;
    });
    if (iter != end)
        return &(*iter);
    return nullptr;
}

template <class _Root, class _Type>
struct TypeHierarchyHelper
{
    template<class _ReverseIter>
    static const _Type* find(_Root const& x, _ReverseIter begin, _ReverseIter end)
    {
        if (begin == end)
            return nullptr;
        auto parent = TypeHierarchyHelper<_Root, typename child_iter<_Type>::ParentType>::find(x, begin+1, end);
        if (!parent)
            return nullptr;
        return impl::find<_Type>(*parent, *begin);
    }
};

template <class _Type>
struct TypeHierarchyHelper<typename child_iter<_Type>::ParentType, _Type>
{
    template<class _Root, class _ReverseIter>
    static const _Type* find(_Root const& parent, _ReverseIter begin, _ReverseIter end)
    {
        if (begin == end)
            return nullptr;
        auto ident = stx::join(end.base(), begin.base(), ".");
        return impl::find<_Type>(parent, ident);
    }
};

} // namespace impl

/**
 * Utility functions for searching metadata.
 *
 * Example:
 *   auto field = zsr::find<zsr::Field>(compound, "member")
 *
 * Example (with recursive field resolution from given package):
 *   auto field = zsr::find<zsr::Field>(package, "compound.member")
 *
 * Example (with absolute path resolution):
 *   auto field = zsr::find<zsr::Field>(zsr::packages(), "package.compound.member")
 */
template <class _Type, class _Root>
const _Type* find(const _Root& r, const std::string& ident)
{
    auto parts = stx::split<std::vector<std::string_view>>(ident, ".");
    return impl::TypeHierarchyHelper<_Root, _Type>::find(r, parts.rbegin(), parts.rend());
}

template <class _Type, class _Root>
auto resolveType(const _Root& r,
                 const zsr::TypeRef& type) -> const _Type*
{
    auto lpkg = zsr::find<zsr::Package>(r, type.package);
    if (!lpkg)
        return nullptr;
    auto sub = zsr::find<zsr::SubType>(*lpkg, type.ident);
    if (sub)
        return resolveType<_Type, _Root>(r, *sub->type);

    return find<_Type>(*lpkg, type.ident);
}

} // namespace zsr
