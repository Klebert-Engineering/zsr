#pragma once

#include "export.hpp"
#include <exception>
#include <string>
#include <string_view>

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
{
    std::string msg;

    explicit Error(std::string what);

    const char* what() const noexcept override;
};

struct ZSR_EXPORT ParameterListTypeError : Error
{
    static ParameterListTypeError listEmpty();
    static ParameterListTypeError listTypeMissmatch(const std::string& expected,
                                                    const std::string& got);

    using Error::Error;
};

struct ZSR_EXPORT IntrospectableCastError : Error
{
    IntrospectableCastError();
    explicit IntrospectableCastError(const Compound* isa,
                                     const Compound* target);
};

struct ZSR_EXPORT VariantCastError : Error
{
    explicit VariantCastError();
};

struct ZSR_EXPORT UnknownIdentifierError : Error
{
    std::string type;
    std::string ident;

    UnknownIdentifierError(std::string type,
                           std::string ident);
};

struct ZSR_EXPORT ParameterizedStructNotAllowedError : Error
{
    explicit ParameterizedStructNotAllowedError(std::string_view ident);
};

} // namespace zsr

#ifdef _MSC_VER
#pragma warning(pop)
#endif
