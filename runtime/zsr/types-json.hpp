#pragma once

#include <zsr/export.hpp>
#include <zsr/types.hpp>
#include <zsr/variant.hpp>
#include <zsr/introspectable.hpp>

#include <nlohmann/json.hpp>

namespace zsr
{

void ZSR_EXPORT to_json(nlohmann::json&, const Introspectable&);

void ZSR_EXPORT to_json(nlohmann::json&, const Variant&);

void ZSR_EXPORT to_json(nlohmann::json&, const ZType&);
void ZSR_EXPORT to_json(nlohmann::json&, const CType&);
void ZSR_EXPORT to_json(nlohmann::json&, const TypeRef&);

void ZSR_EXPORT to_json(nlohmann::json&, const SubType&);
void ZSR_EXPORT to_json(nlohmann::json&, const Constant&);

void ZSR_EXPORT to_json(nlohmann::json&, const BitmaskValue&);
void ZSR_EXPORT to_json(nlohmann::json&, const Bitmask&);

void ZSR_EXPORT to_json(nlohmann::json&, const EnumerationItem&);
void ZSR_EXPORT to_json(nlohmann::json&, const Enumeration&);

void ZSR_EXPORT to_json(nlohmann::json&, const Parameter&);
void ZSR_EXPORT to_json(nlohmann::json&, const Field&);
void ZSR_EXPORT to_json(nlohmann::json&, const Function&);
void ZSR_EXPORT to_json(nlohmann::json&, const Compound&);

void ZSR_EXPORT to_json(nlohmann::json&, const ServiceMethod&);
void ZSR_EXPORT to_json(nlohmann::json&, const Service&);

void ZSR_EXPORT to_json(nlohmann::json&, const Package&);

}

/**
 * Foreign type serializers
 *
 * See: https://github.com/nlohmann/json#how-do-i-convert-third-party-types
 */
namespace nlohmann
{

template <>
struct adl_serializer<zserio::BitBuffer> {
    static void to_json(json& j, const zserio::BitBuffer& b) {
        j = std::vector<uint8_t>(b.getBuffer(),
                                 b.getBuffer() + b.getByteSize());
    }

    static void from_json(const json& j, zserio::BitBuffer& b) {
        b = zserio::BitBuffer(j.get<std::vector<uint8_t>>());
    }
};

}
