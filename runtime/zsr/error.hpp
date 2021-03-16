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
{
    std::string msg;

    explicit Error(std::string what);

    const char* what() const noexcept override;
};

struct ZSR_EXPORT ParameterListTypeError : public Error
{
    static ParameterListTypeError listEmpty();
    static ParameterListTypeError listTypeMissmatch(const std::string& expected,
                                                    const std::string& got);

    using Error::Error;
};

struct ZSR_EXPORT IntrospectableCastError : public Error
{
    IntrospectableCastError();
    explicit IntrospectableCastError(const Compound* isa,
                                     const Compound* target);
};

struct ZSR_EXPORT VariantCastError : public Error
{
    explicit VariantCastError();
};

struct ZSR_EXPORT UnknownIdentifierError : public Error
{
    std::string identifier;
    std::string identifierType;

    explicit UnknownIdentifierError(std::string const& identifierType, std::string const& identifier);
};

struct ZSR_EXPORT ParameterizedStructNotAllowedError : public Error
{
    explicit ParameterizedStructNotAllowedError(std::string const& identifier);
};

} // namespace zsr

#ifdef _MSC_VER
#pragma warning(pop)
#endif
