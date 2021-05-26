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

#include <sofa/simulation/mechanicalvisitor/MechanicalPropagateOnlyPositionVisitor.h>

namespace sofa::simulation::mechanicalvisitor
{

MechanicalPropagateOnlyPositionVisitor::MechanicalPropagateOnlyPositionVisitor(const sofa::core::MechanicalParams* mparams, SReal t, core::MultiVecCoordId x, bool m )
        : MechanicalVisitor(mparams) , t(t), x(x), ignoreMask(m)
{
#ifdef SOFA_DUMP_VISITOR_INFO
    setReadWriteVectors();
#endif
}

Visitor::Result MechanicalPropagateOnlyPositionVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* /*mm*/)
{
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalPropagateOnlyPositionVisitor::fwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    if (!ignoreMask)
    {
        ForceMaskActivate(map->getMechFrom() );
        ForceMaskActivate(map->getMechTo() );
    }
    map->apply(mparams, x, x);

    if (!ignoreMask)
    {
        ForceMaskDeactivate( map->getMechTo() );
    }

    return RESULT_CONTINUE;
}

void MechanicalPropagateOnlyPositionVisitor::bwdMechanicalState(simulation::Node* , core::behavior::BaseMechanicalState* mm)
{
    mm->forceMask.activate(false);
}

std::string MechanicalPropagateOnlyPositionVisitor::getInfos() const
{
    std::string name="x["+x.getName()+"]";
    if (ignoreMask) name += " Mask DISABLED";
    else            name += " Mask ENABLED";
    return name;
}
}