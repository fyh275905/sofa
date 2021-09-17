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
#include <sofa/simulation/PropagateEventVisitor.h>
#include <sofa/simulation/Node.h>

namespace sofa::simulation
{

PropagateEventVisitor::PropagateEventVisitor(const core::ExecParams* params, sofa::core::objectmodel::Event* e)
    : sofa::simulation::Visitor(params)
    , m_event(e)
{}

PropagateEventVisitor::~PropagateEventVisitor()
{}

Visitor::Result PropagateEventVisitor::processNodeTopDown(simulation::Node* node)
{
    // make a copy of the objects list so that the list in the Node can be modified by this event (e.g. by adding
    // or removing an object)
    const auto objects = node->object;

    for_each(this, node, objects, &PropagateEventVisitor::processObject);

    if( m_event->isHandled() )
        return Visitor::RESULT_PRUNE;

    return Visitor::RESULT_CONTINUE;
}

void PropagateEventVisitor::processObject(simulation::Node* node, core::objectmodel::BaseObject* obj)
{
    if (processedObjects.insert(obj).second)
    {
        const auto children = obj->getSlaves();
        for (auto child : children)
        {
            if (child)
            {
                processObject(node, child.get());
            }
        }
        if( obj->f_listening.getValue() == true)
            obj->handleEvent( m_event );
    }
}


}

