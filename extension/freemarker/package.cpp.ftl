#include "${filename}.hpp"

#include <cstring>

/* Public */
#include "zsr/types.hpp"
#include "zsr/error.hpp"
#include "zsr/find.hpp"

/* Private */
using Type2Compound = std::unordered_map<std::type_index, const zsr::Compound*>;

#include "reflection-traits.hpp" /* generated */

#include "type-traits.hpp"
#include "tmp-helper.hpp"
#include "parameterlist-helper.hpp"
#include "field-accessor-helper.hpp"
#include "c-type-helper.hpp"

#include "reflection-macros.hpp"

#ifdef _MSC_VER
#pragma warning(disable:4505) /* MSVC does not honor the unused attribute */
#pragma warning(disable:4100) /* Unreferenced parameter */
#endif

<#list includes as include>
#include "${include}"
</#list>

<#list namespaces as ns>
namespace ${ns} {}
</#list>

namespace zsr_gen
{

void ${packageInitializer}(std::deque<zsr::Package>& pkgs,
                           Type2Compound& t2c)
{
    (void)pkgs;
    (void)t2c;

    <#list macros as macro>
    ${macro}
    </#list>
}

}
