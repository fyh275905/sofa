#include <sofa/component/topology/dynamiccontainer/init.h>

namespace sofa::component::topology::dynamiccontainer
{
    
extern "C" {
    SOFA_EXPORT_DYNAMIC_LIBRARY void initExternalModule();
    SOFA_EXPORT_DYNAMIC_LIBRARY const char* getModuleName();
}

void initExternalModule()
{
    static bool first = true;
    if (first)
    {
        first = false;
    }
}

const char* getModuleName()
{
    return MODULE_NAME;
}

void init()
{
    initExternalModule();
}

} // namespace sofa::component::topology::dynamiccontainer
