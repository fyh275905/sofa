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
#include <sofa/component/topology/ManifoldEdgeSetTopologyModifier.h>
#include <sofa/component/topology/ManifoldEdgeSetTopologyModifier.inl>

#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace topology
{
using namespace sofa::defaulttype;
int ManifoldEdgeSetTopologyModifierClass = core::RegisterObject("ManifoldEdge set topology modifier")
#ifndef SOFA_FLOAT
        .add< ManifoldEdgeSetTopologyModifier<Vec3dTypes> >()
        .add< ManifoldEdgeSetTopologyModifier<Vec2dTypes> >()
        .add< ManifoldEdgeSetTopologyModifier<Vec1dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< ManifoldEdgeSetTopologyModifier<Vec3fTypes> >()
        .add< ManifoldEdgeSetTopologyModifier<Vec2fTypes> >()
        .add< ManifoldEdgeSetTopologyModifier<Vec1fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class ManifoldEdgeSetTopologyModifier<Vec3dTypes>;
template class ManifoldEdgeSetTopologyModifier<Vec2dTypes>;
template class ManifoldEdgeSetTopologyModifier<Vec1dTypes>;
#endif

#ifndef SOFA_DOUBLE
template class ManifoldEdgeSetTopologyModifier<Vec3fTypes>;
template class ManifoldEdgeSetTopologyModifier<Vec2fTypes>;
template class ManifoldEdgeSetTopologyModifier<Vec1fTypes>;
#endif

} // namespace topology

} // namespace component

} // namespace sofa

