#include "zsr/error.hpp"
#include "zsr/types.hpp"

#include "stx/format.h"

namespace zsr {

Error::Error(std::string what)
    : msg(std::move(what))
{}

const char* Error::what() const noexcept
{
    return msg.c_str();
}

ParameterListTypeError ParameterListTypeError::listEmpty()
{
    return ParameterListTypeError("Internal type list unset");
}

ParameterListTypeError ParameterListTypeError::listTypeMissmatch(const std::string& expected,
                                                                 const std::string& got)
{
    return ParameterListTypeError(stx::format(
        "Parameter list type error; expected '{}', got '{}'", expected, got));
}

IntrospectableCastError::IntrospectableCastError()
    : Error("Introspectable cast error")
{}

IntrospectableCastError::IntrospectableCastError(const Compound* isa,
                                                 const Compound* target)
    : Error(stx::format("Introspectable cast error from '{}' to '{}'",
                        isa ? isa->ident.c_str() : "<null>",
                        target ? target->ident.c_str() : "<null>"))
{}

VariantCastError::VariantCastError()
    : Error("Variant cast error")
{}

UnknownIdentifierError::UnknownIdentifierError(std::string type,
                                               std::string ident)
    : Error(stx::format("Could not find '{}::{}'", type, ident))
    , type(std::move(type))
    , ident(std::move(ident))
{}

ParameterizedStructNotAllowedError::ParameterizedStructNotAllowedError(std::string_view ident)
    : Error(stx::format("Refusing to construct '{}' without parameters.", ident))
{}

}
