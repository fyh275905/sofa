/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, v16.12                  *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_PENALITYCONTACTFORCEFIELD_CPP
#include <SofaObjectInteraction/PenalityContactForceField.inl>
#include <sofa/core/behavior/PairInteractionForceField.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace interactionforcefield
{

using namespace sofa::defaulttype;



SOFA_DECL_CLASS(PenalityContactForceField)

// Register in the Factory
int PenalityContactForceFieldClass = core::RegisterObject("Contact using repulsive springs")
#ifndef SOFA_FLOAT
        .add< PenalityContactForceField<Vec3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< PenalityContactForceField<Vec3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_OBJECT_INTERACTION_API PenalityContactForceField<Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_OBJECT_INTERACTION_API PenalityContactForceField<Vec3fTypes>;
#endif

} // namespace interactionforcefield

} // namespace component

} // namespace sofa

