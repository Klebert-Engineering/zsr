/* zserio generated source */
#include "zsr/reflection-main.hpp"

#include <memory>
#include <deque>

<#list packageFilenames as filename>
#include "${filename}.hpp"
</#list>

namespace zsr
{
const std::deque<zsr::Package>& packages()
{
    static std::unordered_map<std::type_index, const zsr::Compound*> type2compound;
    static auto gpkgs = std::make_unique<std::deque<zsr::Package>>(
        ([&]() -> std::deque<zsr::Package> {
            std::deque<zsr::Package> pkgs;

            <#list packageInitializers as package>
            zsr_gen::${package}(pkgs, type2compound);
            </#list>

            return pkgs;
        })()
    );

    return *gpkgs;
}
}
