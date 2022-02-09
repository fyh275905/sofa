/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/component/topology/container/init.h>

#include <sofa/component/topology/container/nondynamic/init.h>
#include <sofa/component/topology/container/dynamic/init.h>
#include <sofa/component/topology/container/grid/init.h>

namespace sofa::component::topology::container
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
        // force dependencies at compile-time
        sofa::component::topology::container::nondynamic::init();
        sofa::component::topology::container::dynamic::init();
        sofa::component::topology::container::grid::init();

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


} // namespace sofa::component::topology::container
