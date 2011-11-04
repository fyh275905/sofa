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
#define SOFA_COMPONENT_FORCEFIELD_MESHSPRINGFORCEFIELD_CPP
#include <sofa/component/interactionforcefield/MeshSpringForceField.inl>
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


SOFA_DECL_CLASS(MeshSpringForceField)

int MeshSpringForceFieldClass = core::RegisterObject("Spring force field acting along the edges of a mesh")
#ifndef SOFA_FLOAT
        .add< MeshSpringForceField<Vec3dTypes> >()
        .add< MeshSpringForceField<Vec2dTypes> >()
        .add< MeshSpringForceField<Vec1dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< MeshSpringForceField<Vec3fTypes> >()
        .add< MeshSpringForceField<Vec2fTypes> >()
        .add< MeshSpringForceField<Vec1fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec3dTypes>;
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec2dTypes>;
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec1dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec3fTypes>;
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec2fTypes>;
template class SOFA_DEFORMABLE_API MeshSpringForceField<Vec1fTypes>;
#endif
} // namespace interactionforcefield

} // namespace component

} // namespace sofa

