#pragma once

#include "export.hpp"
#include <exception>
#include <string>

#ifdef _MSC_VER
/* Disable warning about inheriting from/using unexported class */
#pragma warning(push)
#pragma warning(disable:4275)
#pragma warning(disable:4251)
#endif

namespace zsr {

struct Compound;

/**
 * Exception base class.
 */
struct ZSR_EXPORT Error : std::exception
{};

struct ZSR_EXPORT ParameterListTypeError : Error
{
    std::string msg;

    static ParameterListTypeError listEmpty();
    static ParameterListTypeError listTypeMissmatch(const std::string& expected,
                                                    const std::string& got);

    ParameterListTypeError(std::string msg);

    const char* what() const noexcept override;
};

struct ZSR_EXPORT IntrospectableCastError : Error
{
    std::string msg;

    IntrospectableCastError();
    explicit IntrospectableCastError(const Compound* isa,
                                     const Compound* target);

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
