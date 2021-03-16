#pragma once

#include "zsr/export.hpp"
#include "zsr/types.hpp"
#include "zsr/introspectable.hpp"
#include "zsr/variant.hpp"
#include "zsr/find.hpp"
#include "zsr/error.hpp"

#include <map>
#include <variant>

namespace zsr
{

class VariantMap;
using VariantOrMap = std::variant<Variant, VariantMap>;
class VariantMap : public std::map<std::string, VariantOrMap> {
    using std::map<std::string, VariantOrMap>::map;
};

ZSR_EXPORT Variant get(Introspectable const& instance, std::string const& fieldName);

ZSR_EXPORT void set(Introspectable& instance, std::string const& fieldName, Variant value);

ZSR_EXPORT void set(Introspectable& instance, VariantMap const& fieldValues);

ZSR_EXPORT Introspectable make(std::deque<Package> const& packages,
                    std::string const& compoundIdentifier);

ZSR_EXPORT Introspectable make(std::deque<Package> const& packages,
                    std::string const& compoundIdentifier,
                    VariantMap const& fieldValues);

}
