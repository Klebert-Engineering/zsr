#include "zsr/error.hpp"
#include "zsr/types.hpp"

namespace zsr {

const char* ParameterListTypeError::what() const noexcept
{
    return "Parameter list type error";
}

IntrospectableCastError::IntrospectableCastError()
{}

IntrospectableCastError::IntrospectableCastError(const Compound* isa,
                                                 const Compound* target)
{
    auto sourceIdent = isa ? isa->ident.c_str() : "<null>";
    auto targetIdent = target ? target->ident.c_str() : "<null>";

    msg = "Introspectable cast error from '";
    msg += sourceIdent;
    msg += "' to '";
    msg += targetIdent;
    msg += "'";
}

const char* IntrospectableCastError::what() const noexcept
{
    if (!msg.empty())
        return msg.c_str();
    return "Introspectable cast error";
}

const char* VariantCastError::what() const noexcept
{
    return "Variant cast error";
}

}
