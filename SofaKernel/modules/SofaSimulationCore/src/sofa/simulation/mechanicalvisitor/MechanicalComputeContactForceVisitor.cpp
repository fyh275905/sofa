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

#include <sofa/simulation/mechanicalvisitor/MechanicalComputeContactForceVisitor.h>

namespace sofa::simulation::mechanicalvisitor
{

Visitor::Result MechanicalComputeContactForceVisitor::fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_PRUNE;
}


Visitor::Result MechanicalComputeContactForceVisitor::fwdMappedMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm)
{
    mm->accumulateForce(this->params, res.getId(mm));
    return RESULT_CONTINUE;
}


void MechanicalComputeContactForceVisitor::bwdMechanicalMapping(simulation::Node* /*node*/, core::BaseMapping* map)
{
    map->applyJT(mparams, res, res);
}

} // namespace sofa::simulation::mechanicalvisitor
