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
// TODO: Backward compability header, to be removed for v21.12
#include <SofaBaseTopology/config.h>
#include <SofaBaseTopology/TopologySubsetData.h>

namespace sofa::component::topology
{

//SOFA_ATTRIBUTE_DISABLED("v21.06", "PR#2114", "This class has been removed. TopologySubsetData should be used instead.")
template< class VecT > using PointSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Point, VecT>;
template< class VecT > using EdgeSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Edge, VecT>;
template< class VecT > using TriangleSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Triangle, VecT>;
template< class VecT > using QuadSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Quad, VecT>;
template< class VecT > using TetrahedronSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Tetrahedron, VecT>;
template< class VecT > using HexahedronSparseData = TopologySubsetData<core::topology::BaseMeshTopology::Hexahedron, VecT>;

} //namespace sofa::component::topology
