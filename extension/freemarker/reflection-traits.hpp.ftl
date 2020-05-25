#include <type_traits>

<#list includes as include>
#include "${include}"
</#list>

namespace zsr
{

/**
 * Traits for checking if a type is zserio generated.
 */
template <class>
struct is_compound : std::false_type {};

template <class>
struct is_enumeration : std::false_type {};

/**
 * Generated code.
 */
<#list compoundTypes as ident>
template <>
struct is_compound<::${ident}> : std::true_type {};
</#list>

<#list enumTypes as ident>
template <>
struct is_enumeration<::${ident}> : std::true_type {};
</#list>

}
