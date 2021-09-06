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
#pragma once

#include <sofa/core/MultiVecId[V_ALL].h>

#include <sofa/simulation/BaseMechanicalVisitor.h>
namespace sofa::simulation::mechanicalvisitor
{


class SOFA_SIMULATION_CORE_API MechanicalMultiVectorToBaseVectorVisitor : public BaseMechanicalVisitor
{
public:
    sofa::core::ConstMultiVecId src;
    sofa::defaulttype::BaseVector *vect;
    const sofa::core::behavior::MultiMatrixAccessor* matrix;
    int offset;

    MechanicalMultiVectorToBaseVectorVisitor(
        const core::ExecParams* params,
        sofa::core::ConstMultiVecId _src, defaulttype::BaseVector * _vect,
        const sofa::core::behavior::MultiMatrixAccessor* _matrix = nullptr );

    Result fwdMechanicalState(simulation::Node* /*node*/, core::behavior::BaseMechanicalState* mm) override;

    /// Return a class name for this visitor
    /// Only used for debugging / profiling purposes
    const char* getClassName() const override { return "MechanicalMultiVector2BaseVectorVisitor"; }
};

} // namespace sofa::simulation::mechanicalvisitor
