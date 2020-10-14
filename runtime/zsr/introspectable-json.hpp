#pragma once

#include <zsr/introspectable.hpp>
#include <zsr/types.hpp>

#include <zsr/speedy-j.hpp>

namespace zsr
{

enum ZSR_EXPORT SerializationOptions
{
    SERIALIZE_DEFAULT =           0u, /// Default serialization without metadata
    SERIALIZE_TYPE =         1u << 0, /// Serialize TypeRef to '__type'
    SERIALIZE_METADATA =     1u << 1, /// Add root object with '__meta' array (see examples)
    SERIALIZE_FUNCTIONS =    1u << 2, /// Serialize function return values
    SERIALIZE_RESOLVE_ENUM = 1u << 3, /// Serialize enum values to {ident, value} objects
    SERIALIZE_BITBUFFER =    1u << 4, /// Serialize bitbuffers as arrays
};

/**
 * Serialize an zsr object to JSON.
 *
 * \param obj   Object to serialize.
 * \param pkgs  Optional package list, needed for type-lookup.
 *              Needed for options: *_RESOLVE_ENUM.
 * \param opt   Serialization options.
 * \return      JSON object
 *
 * \example without metadata
 *   zserio:
 *     struct B {
 *       int32 a;
 *     };
 *     struct A {
 *       B a;
 *     };
 *
 *   JSON:
 *     {
 *       a: {
 *         a: 123
 *       }
 *     }
 *
 * \example same zserio object with metadata
 *   JSON:
 *     {
 *       "__meta": [ ← Root __meta array, containing all meta objects
 *         {...}, {...}
 *       ],
 *       "__object": {
 *         "__meta": 0, ← Index to root __meta array
 *         a: {
 *           "__meta": 1,
 *           a: 123
 *         }
 *       }
 *     }
 */
speedyj::Stream ZSR_EXPORT serialize(const zsr::Introspectable& obj,
                                     const std::deque<zsr::Package>* pkgs = nullptr,
                                     SerializationOptions opts = SERIALIZE_DEFAULT);

}
