/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#define SOFA_COMPONENT_CONSTRAINT_ATTACHCONSTRAINT_CPP
#include <sofa/component/constraint/AttachConstraint.inl>
#include <sofa/core/componentmodel/behavior/PairInteractionConstraint.inl>
#include <sofa/core/ObjectFactory.h>

#include <sofa/simulation/common/Node.h>
#include <sofa/component/mass/UniformMass.h>

namespace sofa
{

namespace component
{

namespace constraint
{

using namespace sofa::defaulttype;
using namespace sofa::helper;

SOFA_DECL_CLASS(AttachConstraint)

int AttachConstraintClass = core::RegisterObject("Attach given pair of particles, projecting the positions of the second particles to the first ones")
#ifndef SOFA_FLOAT
        .add< AttachConstraint<Vec3dTypes> >()
        .add< AttachConstraint<Vec2dTypes> >()
        .add< AttachConstraint<Vec1dTypes> >()
        .add< AttachConstraint<Rigid3dTypes> >()
        .add< AttachConstraint<Rigid2dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< AttachConstraint<Vec3fTypes> >()
        .add< AttachConstraint<Vec2fTypes> >()
        .add< AttachConstraint<Vec1fTypes> >()
        .add< AttachConstraint<Rigid3fTypes> >()
        .add< AttachConstraint<Rigid2fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec3dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec2dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec1dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Rigid3dTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Rigid2dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec3fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec2fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Vec1fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Rigid3fTypes>;
template class SOFA_COMPONENT_CONSTRAINT_API AttachConstraint<Rigid2fTypes>;
#endif
} // namespace constraint

} // namespace component

} // namespace sofa

