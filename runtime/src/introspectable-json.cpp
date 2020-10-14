#include <zsr/introspectable-json.hpp>

#include <zsr/types.hpp>
#include <zsr/variant.hpp>
#include <zsr/find.hpp>
#include <zsr/reflection-main.hpp>
#include <zsr/types-json.hpp>

namespace
{

template <class _Type>
auto resolveEnum(const zsr::Enumeration& e,
                 const _Type& v) -> std::string
{
    for (auto&& i : e.items) {
        if (auto iv = i.value.get<_Type>())
            if (*iv == v)
                return i.ident;
    }

    return std::to_string(v);
}

template <>
auto resolveEnum(const zsr::Enumeration& e,
                 const std::string& v) -> std::string
{
    for (auto&& i : e.items) {
        if (auto iv = i.value.get<std::string>())
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
    speedyj::Stream& s;      /* Json stream */
    std::vector<const zsr::Compound*>& metaTypes;

    const std::deque<zsr::Package>* pkgs;
    const zsr::SerializationOptions opts;
    const _Context* context; /* Object meta type being visited */

    JsonVisitor(speedyj::Stream& s,
                std::vector<const zsr::Compound*>& metaTypes,
                const _Context* ctx,
                const std::deque<zsr::Package>* pkgs,
                zsr::SerializationOptions opts)
        : s(s)
        , metaTypes(metaTypes)
        , pkgs(pkgs)
        , opts(opts)
        , context(ctx)
    {}

    auto visit(const zsr::Variant& v)
    {
        zsr::visit(v, *this);
    }

    /* Visitor functions */

    void operator()(const zsr::Introspectable& obj)
    {
        speedyj::ScopedObject _(s);

        auto meta = obj.meta();
        for (const auto& field : meta->fields) {
            if (field.get) {
                auto&& key = field.ident;

                const auto isSet = !field.has || field.has(obj);
                if (isSet) {
                    try {
                        const auto& value = field.get(obj);

                        s << key;
                        JsonVisitor<zsr::Field>(s, metaTypes, &field, pkgs, opts)
                            .visit(value);
                    } catch (...) {}
                } else {
                    s << key << speedyj::Null;
                }
            }
        }

        if (opts & zsr::SERIALIZE_FUNCTIONS) {
            for (const auto& fun : meta->functions) {
                auto&& key = fun.ident;

                try {
                    const auto& value = fun.call(obj);

                    s << key;
                    JsonVisitor<zsr::Function>(s, metaTypes, &fun, pkgs, opts)
                        .visit(value);
                } catch (...) {}
            }
        }

        if (opts & zsr::SERIALIZE_METADATA) {
            /* Add the index to this objects metadata
             * in the metadata-list. */
            auto i = std::find(metaTypes.begin(), metaTypes.end(), obj.meta());
            if (i != metaTypes.end()) {
                s << "__meta" << std::distance(metaTypes.begin(), i);
            } else {
                s << "__meta" << metaTypes.size();
                metaTypes.push_back(obj.meta());
            }
        }
    }

    void operator()(const zserio::BitBuffer& v)
    {
        if (opts & zsr::SERIALIZE_BITBUFFER) {
            s << v;
        } else {
            s << speedyj::Null;
        }
    }

    template <class _Type>
    void operator()(const _Type& v)
    {
        if (opts & zsr::SERIALIZE_RESOLVE_ENUM) {
            if (auto e = getType<zsr::Enumeration>(pkgs, context)) {
                s << speedyj::Object
                  << "ident" << resolveEnum(*e, v)
                  << "value" << v
                  << speedyj::End;
            } else {
                s << v;
            }
        } else {
            s << v;
        }
    }

    template <class _Type>
    void operator()(const std::vector<_Type>& vec)
    {
        speedyj::ScopedArray _(s);

        std::for_each(vec.begin(),
                      vec.end(),
                      [this](const auto& item) {
                          JsonVisitor<_Context>(s, metaTypes, context, pkgs, opts)
                              .visit(item);
                      });
    }
};

}

namespace zsr
{

speedyj::Stream serialize(const Introspectable& i,
                          const std::deque<zsr::Package>* pkgs,
                          SerializationOptions opts)
{
    std::vector<const zsr::Compound*> metaTypes;

    auto s = speedyj::Stream();

    if (opts & zsr::SERIALIZE_METADATA)
        s << speedyj::Object
          << "__object";

    JsonVisitor<void>(s, metaTypes, nullptr, pkgs, opts).visit(i);

    if (opts & zsr::SERIALIZE_METADATA) {
        s << "__meta"
          << speedyj::Array;
        std::for_each(metaTypes.begin(), metaTypes.end(),
                      [&](const auto compound) {
                          s << *compound;
                      });
        s << speedyj::End;
        s << speedyj::End;
    }

    return s;
}

}
