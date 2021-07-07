#pragma once

#include <zsr/export.hpp>
#include <zsr/types.hpp>
#include <zsr/variant.hpp>
#include <zsr/introspectable.hpp>

#include "speedyj/speedyj.hpp"

namespace zsr
{
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Introspectable&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Variant&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const ZType::Type&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const ZType&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const CType::Type&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const CType&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const TypeRef&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const SubType&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Constant&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const BitmaskValue&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Bitmask&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const EnumerationItem&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Enumeration&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Parameter&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Field&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Function&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Compound&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const ServiceMethod&);
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Service&);

ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const Package&);
}

namespace zserio
{
ZSR_EXPORT speedyj::Stream& operator<<(speedyj::Stream&, const BitBuffer&);
}
