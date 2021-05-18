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

#include <sofa/simulation/mechanicalvisitor/MechanicalApplyProjectiveConstraint_ToMatrixVisitor.h>

#include <sofa/core/behavior/BaseProjectiveConstraintSet.h>

namespace sofa::simulation::mechanicalvisitor
{

MechanicalApplyProjectiveConstraint_ToMatrixVisitor::MechanicalApplyProjectiveConstraint_ToMatrixVisitor(
        const core::MechanicalParams *mparams, const sofa::core::behavior::MultiMatrixAccessor *_matrix)
        : MechanicalVisitor(mparams) ,  matrix(_matrix) //,m(_m),b(_b),k(_k)
{
}

Visitor::Result MechanicalApplyProjectiveConstraint_ToMatrixVisitor::fwdMechanicalState(simulation::Node *,
                                                                                        core::behavior::BaseMechanicalState *)
{
    //ms->setOffset(offsetOnExit);
    return RESULT_CONTINUE;
}

Visitor::Result MechanicalApplyProjectiveConstraint_ToMatrixVisitor::fwdProjectiveConstraintSet(simulation::Node *,
                                                                                                core::behavior::BaseProjectiveConstraintSet *c)
{
    if (matrix != nullptr)
    {
        c->applyConstraint(this->mparams, matrix);
    }

    return RESULT_CONTINUE;
}
}