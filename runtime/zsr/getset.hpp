#pragma once

#include "zsr/export.hpp"
#include "zsr/types.hpp"
#include "zsr/introspectable.hpp"
#include "zsr/variant.hpp"
#include "zsr/find.hpp"
#include "zsr/error.hpp"

#include <map>
#include <variant>
#include <string>
#include <string_view>

namespace zsr
{

class VariantMap;
using VariantOrMap = std::variant<Variant, VariantMap>;
class VariantMap : public std::map<std::string, VariantOrMap> {
    using std::map<std::string, VariantOrMap>::map;
};

ZSR_EXPORT Variant get(const Introspectable& instance,
                       std::string_view path);

ZSR_EXPORT Introspectable& set(Introspectable& instance,
                               std::string_view path,
                               Variant value);

ZSR_EXPORT Introspectable& set(Introspectable& instance,
                               VariantMap values);

template <class _Root>
Introspectable make(const _Root& root,
                    std::string_view ident)
{
    if (auto compound = find<Compound>(root, ident))
        return compound->alloc();

    throw UnknownIdentifierError("compound", std::string(ident));
}

template <class _Root>
Introspectable make(const _Root& root,
                    std::string_view ident,
                    VariantMap values)
{
    auto result = make(root, ident);

    if (result.meta()->initialize)
        throw ParameterizedStructNotAllowedError(std::string(ident));

    return set(result, std::move(values));
}

}
