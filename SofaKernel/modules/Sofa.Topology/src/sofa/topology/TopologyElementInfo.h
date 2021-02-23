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

#include <sofa/topology/config.h>

#include <sofa/topology/geometry/Point.h>
#include <sofa/topology/geometry/Edge.h>
#include <sofa/topology/geometry/Triangle.h>
#include <sofa/topology/geometry/Quad.h>
#include <sofa/topology/geometry/Pentahedron.h>
#include <sofa/topology/geometry/Tetrahedron.h>
#include <sofa/topology/geometry/Pyramid.h>
#include <sofa/topology/geometry/Hexahedron.h>

namespace sofa::topology
{

template<class TopologyElement>
struct TopologyElementInfo;

#ifndef SOFA_TOPOLOGY_TOPOLOGYELEMENTINFO_DEFINITION
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Point>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Edge>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Triangle>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Quad>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Pentahedron>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Tetrahedron>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Pyramid>;
template SOFA_TOPOLOGY_API struct TopologyElementInfo<geometry::Hexahedron>;
#endif

} // namespace sofa::topology
