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

}
