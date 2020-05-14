#pragma once

#include "export.hpp"
#include <vector>

namespace zsr {

struct Package;

ZSR_EXPORT const std::vector<const Package*>& packages();

}
