#pragma once

#include <exception>

namespace zsr {

/**
 * Exception base class.
 */
struct Error : std::exception
{};

struct ParameterListTypeError : Error
{
    const char* what() const noexcept override;
};

struct IntrospectableCastError : Error
{
    const char* what() const noexcept override;
};

} // namespace zsr
