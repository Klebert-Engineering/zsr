#pragma once

#include <deque>
#include <unordered_map>
#include <typeindex>

#include "zsr/types.hpp"

namespace zsr_gen
{

void ${packageInitializer}(std::deque<zsr::Package>&,
                           std::unordered_map<std::type_index, const zsr::Compound*>&);

}
