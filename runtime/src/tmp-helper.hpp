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

} // namespace zsr
