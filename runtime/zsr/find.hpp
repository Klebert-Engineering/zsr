#pragma once

#include "zsr/types.hpp"

namespace zsr {
namespace impl {

template <class>
struct ZSR_EXPORT child_iter
{};

template <>
struct ZSR_EXPORT child_iter<zsr::Package>
{
    static auto get(const std::deque<zsr::Package>& r)
    {
        return std::make_pair(r.cbegin(), r.cend());
    }
};

#define DECL_ITER(TYPE, PARENT_TYPE, LIST)                                   \
    template <>                                                              \
    struct ZSR_EXPORT child_iter<TYPE>                                       \
    {                                                                        \
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

} // namespace impl

/**
 * Utility functions for searching metadata.
 *
 * Example:
 *   auto field = zsr::find<zsr::Field>(compound, "name")
 */
template <class _Type, class _Root>
const _Type* find(const _Root& r, const std::string& ident)
{
    auto [begin, end] = impl::child_iter<_Type>::get(r);
    auto iter = std::find_if(begin, end, [&](const auto& c) {
        return c.ident == ident;
    });
    if (iter != end)
        return &(*iter);
    return nullptr;
}

/**
 * Recursive resolve typeref.
 */
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

/**
 * Find zsr object by '.' separated class-path.
 * Resolves subtypes recursively
 *
 * Example:
 *   auto found = zsr::findPath<zsr::Service>(zsr::packages(),
 *                                            "my.package.sub.Service");
 */
template <class _Type, class _Root>
static auto findPath(const _Root& r,
                     const std::string& path) -> const _Type*
{
    auto prefix = std::string_view(path);
    auto pkg_iter = std::find_if(r.begin(),
                                 r.end(),
                                 [&prefix](const zsr::Package& package) {
                                     auto& ident = package.ident;
                                     return
                                         ident.size() < prefix.size() &&
                                         prefix.at(ident.size()) == '.' &&
                                         prefix.compare(0, ident.size(), ident) == 0;
                                 });
    if (pkg_iter == r.end())
        return nullptr;

    auto pkg = &static_cast<const zsr::Package&>(*pkg_iter);
    prefix.remove_prefix(pkg->ident.size() + 1);

    return zsr::find<_Type>(*pkg, std::string{prefix});
}

} // namespace zsr
