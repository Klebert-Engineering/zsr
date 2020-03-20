#include "zsr/types.hpp"

namespace zsr {

Registry& registry() {
    static Registry reg;
    return reg;
}

}
