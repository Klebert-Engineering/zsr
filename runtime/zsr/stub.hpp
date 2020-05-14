#pragma once

#include "export.hpp"
#include <deque>

namespace zsr {

struct Package;

ZSR_EXPORT const std::deque<Package>& packages();

}
