#include <type_traits>

namespace zsr {

template <class _Type>
void check_isa(const zsr::Introspectable& i) noexcept(false)
{
    if (zsr::meta_for_compound<_Type>::ptr != i.meta() && i.meta())
        throw zsr::IntrospectableCastError{};
}

template <class _Compound>
_Compound& introspectable_cast(zsr::Introspectable& i)
{
    check_isa<_Compound>(i);
    return *i.obj->as<_Compound>().obj;
}

template <class _Compound>
const _Compound& introspectable_cast(const zsr::Introspectable& i)
{
    check_isa<_Compound>(i);
    return *i.obj->as<_Compound>().obj;
}

template <class _Compound>
std::shared_ptr<_Compound>
shared_introspectable_cast(const zsr::Introspectable& i)
{
    check_isa<_Compound>(i);
    return i.obj->as<_Compound>().obj;
}

} // namespace zsr
