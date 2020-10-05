#include <zsr/introspectable-json.hpp>

#include <zsr/types.hpp>
#include <zsr/variant.hpp>
#include <zsr/find.hpp>
#include <zsr/reflection-main.hpp>
#include <zsr/types-json.hpp>

namespace nlo = nlohmann;

namespace
{

template <class _Type>
auto resolveEnum(const zsr::Enumeration& e,
                 const _Type& v) -> nlo::json
{
    for (auto&& i : e.items) {
        if (auto iv = i.value.get<_Type>())
            if (*iv == v)
                return i.ident;
    }

    return v;
}

template <class _Target>
auto getType(const std::deque<zsr::Package>*,
             const void*) -> const _Target*
{
    return nullptr;
}

template <class _Target>
auto getType(const std::deque<zsr::Package>* pkgs,
             const zsr::Field* f) -> const _Target*
{
    if (f && f->type && pkgs)
        return zsr::resolveType<_Target>(*pkgs,
                                         *f->type);
    return nullptr;
}

template <class _Target>
auto getType(const std::deque<zsr::Package>* pkgs,
             const zsr::Function* f) -> const _Target*
{
    if (f && f->type && pkgs)
        return zsr::resolveType<_Target>(*pkgs,
                                         *f->type);
    return nullptr;
}

template <class _Context>
struct JsonVisitor
{
    const std::deque<zsr::Package>* pkgs;
    const zsr::SerializationOptions opts;
    const _Context* context; /* Object meta type being visited */
    nlo::json j;             /* Json result value */

    JsonVisitor(const _Context* ctx,
                const std::deque<zsr::Package>* pkgs,
                zsr::SerializationOptions opts)
        : pkgs(pkgs)
        , opts(opts)
        , context(ctx)
    {}

    auto visit(const zsr::Variant& v) -> nlo::json
    {
        zsr::visit(v, *this);
        return j;
    }

    /* Visitor functions */

    void operator()(const zsr::Introspectable& obj)
    {
        auto meta = obj.meta();
        for (const auto& field : meta->fields) {
            if (field.get) {
                auto&& key = field.ident;

                const auto isSet = !field.has || field.has(obj);
                if (isSet)
                    j[key] = JsonVisitor<zsr::Field>(&field, pkgs, opts)
                        .visit(field.get(obj));
                else
                    j[key] = nullptr;
            }
        }

        if (opts & zsr::SERIALIZE_FUNCTIONS) {
            for (const auto& fun : meta->functions) {
                auto&& key = fun.ident;
                auto&& value = fun.call(obj);

                j[key] = JsonVisitor<zsr::Function>(&fun, pkgs, opts).visit(value);
            }
        }

        if (opts & zsr::SERIALIZE_TYPE) {
            j["__type"] = {
                {"package", obj.meta()->parent.ident},
                {"ident", obj.meta()->ident}
            };
        }

        if (opts & zsr::SERIALIZE_METADATA) {
            j["__meta"] = *obj.meta();
        }
    }

    void operator()(const zserio::BitBuffer& v)
    {
        if (opts & zsr::SERIALIZE_BITBUFFER) {
            j = v;
        }
    }

    template <class _Type>
    void operator()(const _Type& v)
    {
        if (opts & zsr::SERIALIZE_RESOLVE_ENUM) {
            if (auto e = getType<zsr::Enumeration>(pkgs, context)) {
                j = {
                    {"ident", resolveEnum(*e, v)},
                    {"value", v}
                };
            } else {
                j = v;
            }
        } else {
            j = v;
        }
    }

    template <class _Type>
    void operator()(const std::vector<_Type>& vec)
    {
        std::vector<nlo::json> array(vec.size());
        std::transform(vec.begin(),
                       vec.end(),
                       array.begin(),
                       [this](const auto& item) {
                           return JsonVisitor<_Context>(context, pkgs, opts)
                               .visit(item);
                       });

        j = std::move(array);
    }
};

}

namespace zsr
{

nlo::json serialize(const Introspectable& i,
                    const std::deque<zsr::Package>* pkgs,
                    SerializationOptions opts)
{
    return JsonVisitor<void>(nullptr, pkgs, opts).visit(i);
}

}
