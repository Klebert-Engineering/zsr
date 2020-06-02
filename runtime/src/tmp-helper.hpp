#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace zsr {

template <class _Type>
void check_isa(const zsr::Introspectable& i,
               const Type2Compound& t2c) noexcept(false)
{
    auto iter = t2c.find(std::type_index(typeid(_Type)));
    if (iter == t2c.end())
        throw zsr::IntrospectableCastError{nullptr, i.meta()};

    if (iter->second != i.meta() && i.meta())
        throw zsr::IntrospectableCastError{iter->second, i.meta()};
}

template <class _Compound>
_Compound& introspectable_cast(zsr::Introspectable& i,
                               const Type2Compound& t2c)
{
    check_isa<_Compound>(i, t2c);
    return *i.obj->as<_Compound>().obj;
}

template <class _Compound>
const _Compound& introspectable_cast(const zsr::Introspectable& i,
                                     const Type2Compound& t2c)
{
    check_isa<_Compound>(i, t2c);
    return *i.obj->as<_Compound>().obj;
}

template <class _Compound>
std::shared_ptr<_Compound>
shared_introspectable_cast(const zsr::Introspectable& i,
                           const Type2Compound& t2c)
{
    check_isa<_Compound>(i, t2c);
    return i.obj->as<_Compound>().obj;
}

template <class _Type, bool _IsCompound = is_compound<_Type>::value>
struct variant_helper
{
    static _Type unpack(const Variant& v,
                        const Type2Compound&)
    {
        if (auto unpacked = v.get<_Type>())
            return *unpacked;

        throw VariantCastError{};
    }

    static Variant pack(_Type v,
                        const Type2Compound&)
    {
        return Variant{v};
    }
};

template <class _Type>
struct variant_helper<_Type, true>
{
    static _Type& unpack(Variant& v,
                         const Type2Compound& t2c)
    {
        if (auto unpacked = v.get<Introspectable>()) {
            return introspectable_cast<_Type>(*unpacked,
                                              t2c);
        }

        throw VariantCastError{};
    }

    static const _Type& unpack(const Variant& v,
                               const Type2Compound& t2c)
    {
        if (auto unpacked = v.get<Introspectable>()) {
            return introspectable_cast<_Type>(*unpacked,
                                              t2c);
        }

        throw VariantCastError{};
    }

    static Variant pack(_Type v,
                        const Type2Compound& t2c)
    {
        auto iter = t2c.find(std::type_index(typeid(_Type)));
        if (iter == t2c.end())
            throw VariantCastError{};

        Introspectable obj(Introspectable::FromObjectTag{},
                           iter->second,
                           std::move(v));

        return Variant{std::move(obj)};
    }
};

template <class _Type>
struct variant_helper<std::vector<_Type>, true>
{
    static _Type unpack(const Variant& v,
                        const Type2Compound& t2c)
    {
        if (auto unpacked = v.get<std::vector<Introspectable>>()) {
            std::vector<_Type> casted;
            casted.reserve(unpacked->size());

            std::transform(unpacked->begin(),
                           unpacked->end(),
                           std::back_inserter(casted),
                           [&t2c](const auto& i) {
                               introspectable_cast<_Type>(i, t2c);
                           });
            return casted;
        }

        throw VariantCastError{};
    }

    static Variant pack(const std::vector<_Type>& v,
                        const Type2Compound& t2c)
    {
        std::vector<Introspectable> values;
        values.reserve(v.size());

        std::transform(v.begin(), v.end(), std::back_inserter(values),
                       [&](const auto& item) {
                           return variant_helper<_Type>
                               ::pack(item, t2c);
                       });

        return Variant{std::move(values)};
    }
};


} // namespace zsr
