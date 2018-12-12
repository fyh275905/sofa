/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define SOFA_COMPONENT_TOPOLOGY_HEXAHEDRONSETTOPOLOGYALGORITHMS_CPP
#include <SofaBaseTopology/HexahedronSetTopologyAlgorithms.inl>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{
namespace component
{
namespace topology
{
using namespace sofa::defaulttype;
int HexahedronSetTopologyAlgorithmsClass = core::RegisterObject("Hexahedron set topology algorithms")
#ifdef SOFA_FLOAT
        .add< HexahedronSetTopologyAlgorithms<Vec3fTypes> >(true) // default template
#else
        .add< HexahedronSetTopologyAlgorithms<Vec3dTypes> >(true) // default template
#ifndef SOFA_DOUBLE
        .add< HexahedronSetTopologyAlgorithms<Vec3fTypes> >() // default template
#endif
#endif
#ifndef SOFA_FLOAT
        .add< HexahedronSetTopologyAlgorithms<Vec2dTypes> >()
        .add< HexahedronSetTopologyAlgorithms<Vec1dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< HexahedronSetTopologyAlgorithms<Vec2fTypes> >()
        .add< HexahedronSetTopologyAlgorithms<Vec1fTypes> >()
#endif
        ;
#ifndef SOFA_FLOAT
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec3dTypes>;
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec2dTypes>;
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec1dTypes>;
#endif

#ifndef SOFA_DOUBLE
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec3fTypes>;
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec2fTypes>;
template class SOFA_BASE_TOPOLOGY_API HexahedronSetTopologyAlgorithms<Vec1fTypes>;
#endif

} // namespace topology

} // namespace component

} // namespace sofa

