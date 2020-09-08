#pragma once

#include <iterator>

namespace zsr {

/**
 * Structure field accessor generation functions
 * for fundamental and zserio generated types.
 */
template <class _Type,
          bool _IsCompound = is_compound<remove_vector_t<_Type>>::value, /* False */
          bool _IsVector = is_vector<_Type>::value> /* False|True */
struct GenFieldAccessorHelper;

template <class _Type, bool _IsVector>
struct GenFieldAccessorHelper<_Type, false, _IsVector>
{
    using Type = std::decay_t<_Type>;

    template <class _Structure, class _GetFun, class _MetaType>
    static auto getFun(_GetFun fun,
                       _MetaType*,
                       const Type2Compound& t2c)
    {
        return [fun, &t2c](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i, t2c);
            return zsr::Variant(fun(obj));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun,
                       _MetaType*,
                       const Type2Compound& t2c)
    {
        return [fun, &t2c](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i, t2c);
            if (auto vv = v.get<Type>())
                fun(obj, std::move(*vv));
            else
                throw zsr::VariantCastError{};
        };
    }
};

template <class _Type>
struct GenFieldAccessorHelper<_Type, true /* IsCompound */, true /* IsVector */>
{
    using Type = std::decay_t<_Type>;

    template <class _Structure, class _GetFun, class _MetaType>
    static auto getFun(_GetFun fun,
                       _MetaType* meta,
                       const Type2Compound& t2c)
    {
        return [fun, meta, &t2c](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i, t2c);
            const auto* vector = &fun(obj);

            std::vector<Introspectable> ret;
            ret.reserve(vector->size());

            std::transform(
                vector->begin(),
                vector->end(),
                std::back_inserter(ret),
                [&](const auto& item) {
                    auto iter = t2c.find(
                        std::type_index(
                            typeid(remove_vector_t<Type>)));
                    assert(iter != t2c.end() &&
                           "No compound for type (vector: true)");

                    return zsr::Introspectable(
                        iter != t2c.end()
                            ? iter->second
                            : (const zsr::Compound*)nullptr,
                        zsr::impl::makeWeakInstance(i.obj, meta, &item));
                });

            return zsr::Variant(std::move(ret));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun,
                       _MetaType* meta,
                       const Type2Compound& t2c)
    {
        return [fun, meta, &t2c](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i, t2c);

            if (auto vector = v.get<std::vector<Introspectable>>()) {
                i.obj->makeChildrenOwning(meta);

                std::vector<remove_vector_t<Type>> zserioVector;
                zserioVector.reserve(vector->size());

                std::transform(
                    vector->begin(),
                    vector->end(),
                    std::back_inserter(zserioVector),
                    [&](const auto& item) {
                        return introspectable_cast<remove_vector_t<Type>>(item, t2c);
                    });

                fun(obj, std::move(zserioVector));
            } else {
                throw zsr::VariantCastError{};
            }
        };
    }
};

template <class _Type>
struct GenFieldAccessorHelper<_Type,
                              true /* IsCompound */,
                              false /* IsVector */>
{
    using Type = std::decay_t<_Type>;

    template <class _Structure, class _GetFun, class _MetaType>
    static auto getFun(_GetFun fun,
                       _MetaType* meta,
                       const Type2Compound& t2c)
    {
        return [fun, meta, &t2c](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i, t2c);
            const auto* value = &fun(obj);

            auto iter = t2c.find(std::type_index(typeid(Type)));
            assert(iter != t2c.end() && "No compound for type (vector: false)");

            if (iter == t2c.end())
                return zsr::Variant{};

            return zsr::Variant(zsr::Introspectable(
                iter->second,
                zsr::impl::makeWeakInstance(i.obj, meta, value)));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun,
                       _MetaType* meta,
                       const Type2Compound& t2c)
    {
        return [fun, meta, &t2c](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i, t2c);
            if (auto vv = v.get<zsr::Introspectable>()) {
                const auto& value = introspectable_cast<Type>(*vv, t2c);

                i.obj->makeChildrenOwning(meta);

                fun(obj, value);
            } else
                throw zsr::VariantCastError{};
        };
    }
};

} // namespace zsr
