#include "zsr/getset.hpp"
#include "zsr/reflection-main.hpp"

namespace zsr
{

namespace impl
{

Introspectable introspectableForFieldOrFunctionPath(
    Introspectable const& parent,
    std::vector<std::string_view> const& path)
{
    // I (kind of) abuse Introspectable as a Box type here -
    // the only alternative I can think of is a unique pointer,
    // but that would also bloat the the code more.
    Introspectable currentInstance = parent;

    for (auto i = 0; i < path.size()-1; ++i) {
        auto field = zsr::find<zsr::Field>(*currentInstance.meta(), std::string(path[i]));

        if (!field)
            throw UnknownIdentifierError(
                "field",
                stx::join(path.begin(), path.begin()+1+i, "."));
        if (auto opt = field->get(currentInstance).get<Introspectable>())
            currentInstance = opt.value();
        else
            throw VariantCastError();
    }

    return currentInstance;
}

}

void set(Introspectable& instance, std::string const& fieldName, Variant value)
{
    auto path = stx::split<std::vector<std::string_view>>(fieldName, ".");
    auto fieldParent = impl::introspectableForFieldOrFunctionPath(instance, path);
    auto field = zsr::find<zsr::Field>(*fieldParent.meta(), std::string(path.back()));
    if (!field)
        throw UnknownIdentifierError("field", fieldName);
    field->set(fieldParent, std::move(value));
}

void set(Introspectable& instance, VariantMap const& fieldValues)
{
    for (auto const& kv : fieldValues) {
        switch (kv.second.index()) {
            case 0: {
                set(instance, kv.first, std::get<Variant>(kv.second));
                break;
            }
            case 1: {
                auto path = stx::split<std::vector<std::string_view>>(kv.first, ".");
                auto fieldParent = impl::introspectableForFieldOrFunctionPath(instance, path);
                auto field = zsr::find<zsr::Field>(*fieldParent.meta(), std::string(path.back()));
                auto fieldIntrospectable = field->get(fieldParent).get<zsr::Introspectable>();
                if (!fieldIntrospectable)
                    throw UnknownIdentifierError("field", kv.first);
                set(*fieldIntrospectable, std::get<VariantMap>(kv.second));
            }
        }
    }
}

Variant get(Introspectable const& instance, std::string const& fieldName)
{
    auto path = stx::split<std::vector<std::string_view>>(fieldName, ".");
    auto fieldParent = impl::introspectableForFieldOrFunctionPath(instance, path);
    auto field = zsr::find<zsr::Field>(*fieldParent.meta(), std::string(path.back()));
    if (field)
        return field->get(fieldParent);
    else {
        auto fn = zsr::find<zsr::Function>(*fieldParent.meta(), std::string(path.back()));
        if (fn) {
            return fn->call(fieldParent);
        }
        else
            throw UnknownIdentifierError("field or function", fieldName);
    }
}

Introspectable make(std::deque<Package> const& packages, std::string const& compoundIdentifier)
{
    auto compound = zsr::find<zsr::Compound>(packages, compoundIdentifier);
    if (!compound)
        throw UnknownIdentifierError("compound", compoundIdentifier);
    if (compound->initialize)
        throw ParameterizedStructNotAllowedError(compoundIdentifier);
    return compound->alloc();
}

Introspectable make(std::deque<Package> const& packages, std::string const& compoundIdentifier, VariantMap const& fieldValues)
{
    auto result = make(packages, compoundIdentifier);
    set(result, fieldValues);
    return result;
}

}
