#include <sofa/component/odesolver/forward/init.h>

namespace sofa::component::odesolver::forward
{
	
extern "C" {
    SOFACOMPONENTODESOLVERFORWARD_API void initExternalModule();
    SOFACOMPONENTODESOLVERFORWARD_API const char* getModuleName();
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

} // namespace sofa::component::odesolver::forward
