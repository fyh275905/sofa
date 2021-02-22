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

#include <sofa/topology/TopologyElementType.h>
#include <sofa/type/stdtype/fixed_array.h>
#include <sofa/helper/vector.h>

#include <climits>
#include <string>

namespace sofa::topology
{

class SOFA_TOPOLOGY_API Topology
{
public:
    /// Topology global typedefs
    using Index = sofa::Index;
    static constexpr Index InvalidID = sofa::InvalidID;

    typedef Index                 ElemID;
    typedef Index                 PointID;
    typedef Index                 EdgeID;
    typedef Index                 TriangleID;
    typedef Index                 QuadID;
    typedef Index                 TetraID;
    typedef Index                 TetrahedronID;
    typedef Index                 HexaID;
    typedef Index                 HexahedronID;
    typedef Index                 PentahedronID;
    typedef Index                 PentaID;
    typedef Index                 PyramidID;

    typedef sofa::helper::vector<Index>                  SetIndex;
    typedef sofa::helper::vector<Index>                  SetIndices;

    typedef PointID                             Point;
    // in the following types, we use wrapper classes to have different types for each element, otherwise Quad and Tetrahedron would be the same
    class Edge : public sofa::type::stdtype::fixed_array<PointID,2>
    {
    public:
        Edge(): sofa::type::stdtype::fixed_array<PointID,2>(Topology::InvalidID, Topology::InvalidID){}
        Edge(PointID a, PointID b) : sofa::type::stdtype::fixed_array<PointID,2>(a,b) {}
    };

    class Triangle : public sofa::type::stdtype::fixed_array<PointID,3>
    {
    public:
        Triangle(): sofa::type::stdtype::fixed_array<PointID,3>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Triangle(PointID a, PointID b, PointID c) : sofa::type::stdtype::fixed_array<PointID,3>(a,b,c) {}
    };

    class Quad : public sofa::type::stdtype::fixed_array<PointID,4>
    {
    public:
        Quad(): sofa::type::stdtype::fixed_array<PointID,4>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Quad(PointID a, PointID b, PointID c, PointID d) : sofa::type::stdtype::fixed_array<PointID,4>(a,b,c,d) {}
    };

    class Tetrahedron : public sofa::type::stdtype::fixed_array<PointID,4>
    {
    public:
        Tetrahedron(): sofa::type::stdtype::fixed_array<PointID,4>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Tetrahedron(PointID a, PointID b, PointID c, PointID d) : sofa::type::stdtype::fixed_array<PointID,4>(a,b,c,d) {}
    };
    typedef Tetrahedron                         Tetra;

    class Pyramid : public sofa::type::stdtype::fixed_array<PointID,5>
    {
    public:
        Pyramid(): sofa::type::stdtype::fixed_array<PointID,5>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Pyramid(PointID a, PointID b, PointID c, PointID d, PointID e) : sofa::type::stdtype::fixed_array<PointID,5>(a,b,c,d,e) {}
    };

    class Pentahedron : public sofa::type::stdtype::fixed_array<PointID,6>
    {
    public:
        Pentahedron(): sofa::type::stdtype::fixed_array<PointID,6>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Pentahedron(PointID a, PointID b, PointID c, PointID d, PointID e, PointID f) : sofa::type::stdtype::fixed_array<PointID,6>(a,b,c,d,e,f) {}
    };
    typedef Pentahedron                          Penta;

    class Hexahedron : public sofa::type::stdtype::fixed_array<PointID,8>
    {
    public:
        Hexahedron(): sofa::type::stdtype::fixed_array<PointID,8>(Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID,
                                                           Topology::InvalidID, Topology::InvalidID, Topology::InvalidID, Topology::InvalidID) {}
        Hexahedron(PointID a, PointID b, PointID c, PointID d,
                   PointID e, PointID f, PointID g, PointID h) : sofa::type::stdtype::fixed_array<PointID,8>(a,b,c,d,e,f,g,h) {}
    };
    typedef Hexahedron                          Hexa;

protected:
    Topology() {}
    ~Topology() {}
public:
    // Declare invalid topology structures filled with Topology::InvalidID
    static const sofa::helper::vector<Topology::Index> InvalidSet;
    static const Edge                                       InvalidEdge;
    static const Triangle                                   InvalidTriangle;
    static const Quad                                       InvalidQuad;
    static const Tetrahedron                                InvalidTetrahedron;
    static const Pyramid                                    InvalidPyramid;
    static const Pentahedron                                InvalidPentahedron;
    static const Hexahedron                                 InvalidHexahedron;
};


template<class TopologyElement>
struct TopologyElementInfo;

template<>
struct TopologyElementInfo<Topology::Point>
{
    static TopologyElementType type() { return TopologyElementType::POINT; }
    static const char* name() { return "Point"; }
};

template<>
struct TopologyElementInfo<Topology::Edge>
{
    static TopologyElementType type() { return TopologyElementType::EDGE; }
    static const char* name() { return "Edge"; }
};

template<>
struct TopologyElementInfo<Topology::Triangle>
{
    static TopologyElementType type() { return TopologyElementType::TRIANGLE; }
    static const char* name() { return "Triangle"; }
};

template<>
struct TopologyElementInfo<Topology::Quad>
{
    static TopologyElementType type() { return TopologyElementType::QUAD; }
    static const char* name() { return "Quad"; }
};

template<>
struct TopologyElementInfo<Topology::Tetrahedron>
{
    static TopologyElementType type() { return TopologyElementType::TETRAHEDRON; }
    static const char* name() { return "Tetrahedron"; }
};

template<>
struct TopologyElementInfo<Topology::Pyramid>
{
    static TopologyElementType type() { return TopologyElementType::PYRAMID; }
    static const char* name() { return "Pyramid"; }
};

template<>
struct TopologyElementInfo<Topology::Pentahedron>
{
    static TopologyElementType type() { return TopologyElementType::PENTAHEDRON; }
    static const char* name() { return "Pentahedron"; }
};

template<>
struct TopologyElementInfo<Topology::Hexahedron>
{
    static TopologyElementType type() { return TopologyElementType::HEXAHEDRON; }
    static const char* name() { return "Hexahedron"; }
};

} // namespace sofa::topology
