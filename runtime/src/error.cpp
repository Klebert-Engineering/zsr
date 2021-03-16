#include "zsr/error.hpp"
#include "zsr/types.hpp"
#include "stx/string.h"

namespace zsr {

Error::Error(std::string what) : msg(std::move(what)) {}

const char* Error::what() const noexcept
{
    return msg.c_str();
}

ParameterListTypeError ParameterListTypeError::listEmpty()
{
    return ParameterListTypeError("Internal type list unset");
}

ParameterListTypeError ParameterListTypeError::listTypeMissmatch(
    const std::string& expected,
    const std::string& got)
{
    return ParameterListTypeError(stx::replace_with(
        "Parameter list type error. Expected '?', got '?'", "?", expected, got));
}

IntrospectableCastError::IntrospectableCastError() : Error("Introspectable cast error")
{}

IntrospectableCastError::IntrospectableCastError(const Compound* isa,
                                                 const Compound* target) :
    Error(
        stx::replace_with("Introspectable cast error from '?' to '?'", "?",
            isa ? isa->ident.c_str() : "<null>",
            target ? target->ident.c_str() : "<null>")
    )
{}

VariantCastError::VariantCastError() : Error("Variant cast error")
{}

UnknownIdentifierError::UnknownIdentifierError(std::string const& identifierType, std::string const& identifier) :
    identifier(identifier),
    identifierType(identifierType),
    Error(stx::replace_with("Could not find ? '?'", "?", identifierType, identifier))
{}

ParameterizedStructNotAllowedError::ParameterizedStructNotAllowedError(std::string const& identifier) :
    Error(stx::replace_with("Refusing to construct ? without parameters.", "?", identifier))
{}

}
