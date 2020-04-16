#include "zsr/error.hpp"

namespace zsr {

const char* ParameterListTypeError::what() const noexcept
{
    return "Parameter list type error";
}

const char* IntrospectableCastError::what() const noexcept
{
    return "Introspectable cast error";
}

const char* VariantCastError::what() const noexcept
{
    return "Variant cast error";
}

}
