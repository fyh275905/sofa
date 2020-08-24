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
#define SOFA_COMPONENT_FORCEFIELD_STIFFSPRINGFORCEFIELD_CPP
#include <SofaDeformable/StiffSpringForceField.inl>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace interactionforcefield
{

using namespace sofa::defaulttype;


// Register in the Factory
int StiffSpringForceFieldClass = core::RegisterObject("Stiff springs for implicit integration")
        .add< StiffSpringForceField<Vec3Types> >()
        .add< StiffSpringForceField<Vec2Types> >()
        .add< StiffSpringForceField<Vec1Types> >()
        .add< StiffSpringForceField<Vec6Types> >()
        .add< StiffSpringForceField<Rigid3Types> >()
        ;
template class StiffSpringForceField<Vec3Types>;
template class StiffSpringForceField<Vec2Types>;
template class StiffSpringForceField<Vec1Types>;
template class StiffSpringForceField<Vec6Types>;
template class StiffSpringForceField<Rigid3Types>;

} // namespace interactionforcefield

} // namespace component

} // namespace sofa

