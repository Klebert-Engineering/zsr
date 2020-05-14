#pragma once

#include "export.hpp"
#include <exception>

#ifdef _MSC_VER
/* Disable warning about inheriting from unexported class */
#pragma warning(push)
#pragma warning(disable:4275)
#endif

namespace zsr {

/**
 * Exception base class.
 */
struct ZSR_EXPORT Error : std::exception
{};

struct ZSR_EXPORT ParameterListTypeError : Error
{
    const char* what() const noexcept override;
};

struct ZSR_EXPORT IntrospectableCastError : Error
{
    const char* what() const noexcept override;
};

struct ZSR_EXPORT VariantCastError : Error
{
    const char* what() const noexcept override;
};

} // namespace zsr

#ifdef _MSC_VER
#pragma warning(pop)
#endif
