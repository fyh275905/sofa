/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#include <sofa/component/forcefield/MeshSpringForceField.inl>
#include <sofa/core/componentmodel/behavior/PairInteractionForceField.inl>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace forcefield
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
template class MeshSpringForceField<Vec3dTypes>;
template class MeshSpringForceField<Vec2dTypes>;
template class MeshSpringForceField<Vec1dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class MeshSpringForceField<Vec3fTypes>;
template class MeshSpringForceField<Vec2fTypes>;
template class MeshSpringForceField<Vec1fTypes>;
#endif
} // namespace forcefield

} // namespace component

} // namespace sofa

