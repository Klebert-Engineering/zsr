#pragma once

#include <zsr/export.hpp>
#include <zsr/types.hpp>
#include <zsr/variant.hpp>
#include <zsr/introspectable.hpp>
#include <zsr/speedy-j.hpp>

namespace zsr
{
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Introspectable&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Variant&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const ZType::Type&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const ZType&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const CType::Type&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const CType&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const TypeRef&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const SubType&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Constant&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const BitmaskValue&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Bitmask&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const EnumerationItem&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Enumeration&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Parameter&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Field&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Function&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Compound&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const ServiceMethod&);
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Service&);

speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const Package&);
}

namespace zserio
{
speedyj::Stream& ZSR_EXPORT operator<<(speedyj::Stream&, const BitBuffer&);
}
