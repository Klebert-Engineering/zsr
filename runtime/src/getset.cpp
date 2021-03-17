#include "zsr/getset.hpp"
#include "zsr/reflection-main.hpp"

#include <iostream>

namespace zsr
{

template <class _Iter>
std::tuple<Introspectable, const Field&>
queryFieldRecursive(const Introspectable& parent, _Iter origBegin, _Iter begin, _Iter end)
{
    if (begin == end)
        throw UnknownIdentifierError("field", stx::join(origBegin, end, ".")); /* Should only happen if path is empty */

    if (auto field = find<Field>(*parent.meta(), *begin)) {
        if (begin + 1 == end)
            return {parent, *field};

        if (auto opt = field->get(parent).template get<Introspectable>())
            return queryFieldRecursive(*opt, origBegin, begin + 1, end);

        throw VariantCastError();
    }

    throw UnknownIdentifierError("field", stx::join(origBegin, begin, "."));
}

auto queryFieldRecursive(const Introspectable& parent, std::string_view path)
{
    auto parts = stx::split<std::vector<std::string_view>>(path, ".");

    return queryFieldRecursive(parent, parts.begin(), parts.begin(), parts.end());
}

Introspectable& set(Introspectable& instance, std::string_view path, Variant value)
{
    auto [parent, field] = queryFieldRecursive(instance, path);

    field.set(parent, std::move(value));
    return instance;
}

Introspectable& set(Introspectable& instance, VariantMap values)
{
    for (auto&& [key, value] : std::move(values)) {
        if (auto v = std::get_if<Variant>(&value)) {
            set(instance, key, std::move(*v));
        }

        if (auto v = std::get_if<VariantMap>(&value)) {
            auto [nested, field] = queryFieldRecursive(instance, key);

            if (auto opt = field.get(nested).get<Introspectable>())
                set(*opt, std::move(*v));
            else
                throw VariantCastError();
        }
    }

    return instance;
}

Variant get(Introspectable const& instance, std::string_view path)
{
    auto [parent, field] = queryFieldRecursive(instance, path);

    return field.get(parent);
}

}
