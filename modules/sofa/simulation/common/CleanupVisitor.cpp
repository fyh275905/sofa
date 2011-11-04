/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/simulation/common/CleanupVisitor.h>
#include <sofa/simulation/common/Node.h>

namespace sofa
{

namespace simulation
{


simulation::Visitor::Result CleanupVisitor::processNodeTopDown(Node* node)
{
    // some object will modify the graph during cleanup (removing other nodes or objects)
    // so we cannot assume that the list of object will stay constant

    std::set<sofa::core::objectmodel::BaseObject*> done; // list of objects we already processed
    bool stop = false;
    while (!stop)
    {
        stop = true;
        std::vector< core::objectmodel::BaseObject* > listObject;
        node->get<core::objectmodel::BaseObject>(&listObject, core::objectmodel::BaseContext::Local);

        for (unsigned int i=0; i<listObject.size(); ++i)
        {
            if (done.insert(listObject[i]).second)
            {
                listObject[i]->cleanup();
                stop = false;
                break; // we have to restart as objects could have been removed anywhere
            }
        }
    }
    return RESULT_CONTINUE;
}

void CleanupVisitor::processNodeBottomUp(Node* /*node*/)
{
}

} // namespace simulation

} // namespace sofa

