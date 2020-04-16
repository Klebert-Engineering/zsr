#pragma once

namespace zsr {

/**
 * Structure field accessor generation functions
 * for fundamental and zserio generated types.
 */
template <class _Type,
          bool _IsCompound = is_compound<remove_vector_t<_Type>>::value,
          bool _IsVector = is_vector<_Type>::value>
struct GenFieldAccessorHelper;

template <class _Type, bool _IsVector>
struct GenFieldAccessorHelper<_Type, false, _IsVector>
{
    using Type = std::decay_t<_Type>;

    template <class _Structure, class _GetFun, class _MetaType>
    static auto getFun(_GetFun fun, _MetaType*)
    {
        return [fun](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i);
            return zsr::Variant(fun(obj));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun, _MetaType*)
    {
        return [fun](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i);
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
    static auto getFun(_GetFun fun, _MetaType* meta)
    {
        return [fun, meta](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i);
            const auto* vector = &fun(obj);

            std::vector<Introspectable> ret;
            ret.reserve(vector->size());

            std::transform(
                vector->begin(),
                vector->end(),
                std::back_inserter(ret),
                [&](const auto& item) {
                    return zsr::Introspectable(
                        meta_for_compound<remove_vector_t<Type>>::ptr,
                        zsr::impl::makeWeakInstance(i.obj, meta, &item));
                });

            return zsr::Variant(std::move(ret));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun, _MetaType* meta)
    {
        return [fun, meta](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i);

            if (auto vector = v.get<std::vector<Introspectable>>()) {
                i.obj->makeChildrenOwning(meta);

                std::vector<remove_vector_t<Type>> zserioVector;
                zserioVector.reserve(vector->size());

                std::transform(
                    vector->begin(),
                    vector->end(),
                    std::back_inserter(zserioVector),
                    [&](const auto& item) {
                        return *item.obj->template as<remove_vector_t<Type>>()
                                    .obj;
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
    static auto getFun(_GetFun fun, _MetaType* meta)
    {
        return [fun, meta](const zsr::Introspectable& i) -> zsr::Variant {
            const auto& obj = introspectable_cast<_Structure>(i);

            const auto* value = &fun(obj);
            return zsr::Variant(zsr::Introspectable(
                meta_for_compound<remove_vector_t<Type>>::ptr,
                zsr::impl::makeWeakInstance(i.obj, meta, value)));
        };
    }

    template <class _Structure, class _SetFun, class _MetaType>
    static auto setFun(_SetFun fun, _MetaType* meta)
    {
        return [fun, meta](zsr::Introspectable& i, zsr::Variant v) {
            auto& obj = introspectable_cast<_Structure>(i);
            if (auto vv = v.get<zsr::Introspectable>()) {
                i.obj->makeChildrenOwning(meta);

                fun(obj, *vv->obj->as<Type>().obj);
            } else
                throw zsr::VariantCastError{};
        };
    }
};

} // namespace zsr
