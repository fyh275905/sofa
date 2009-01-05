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
#ifndef SOFA_COMPONENT_TOPOLOGY_HEXAHEDRONSETGEOMETRYALGORITHMS_H
#define SOFA_COMPONENT_TOPOLOGY_HEXAHEDRONSETGEOMETRYALGORITHMS_H

#include <sofa/component/base/topology/QuadSetGeometryAlgorithms.h>

namespace sofa
{
namespace component
{
namespace topology
{
using core::componentmodel::topology::BaseMeshTopology;
typedef BaseMeshTopology::HexaID HexaID;
typedef BaseMeshTopology::Hexa Hexa;
typedef BaseMeshTopology::SeqHexas SeqHexas;
typedef BaseMeshTopology::VertexHexas VertexHexas;
typedef BaseMeshTopology::EdgeHexas EdgeHexas;
typedef BaseMeshTopology::QuadHexas QuadHexas;
typedef BaseMeshTopology::HexaEdges HexaEdges;
typedef BaseMeshTopology::HexaQuads HexaQuads;

typedef Hexa Hexahedron;
typedef HexaEdges HexahedronEdges;
typedef HexaQuads HexahedronQuads;

/**
* A class that provides geometry information on an HexahedronSet.
*/
template < class DataTypes >
class HexahedronSetGeometryAlgorithms : public QuadSetGeometryAlgorithms<DataTypes>
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;

    HexahedronSetGeometryAlgorithms()
        : QuadSetGeometryAlgorithms<DataTypes>()
    {}

    virtual ~HexahedronSetGeometryAlgorithms() {}

    void computeHexahedronAABB(const HexaID h, Coord& minCoord, Coord& maxCoord) const;

    void computeHexahedronRestAABB(const HexaID h, Coord& minCoord, Coord& maxCoord) const;

    Coord computeHexahedronCenter(const HexaID h) const;

    Coord computeHexahedronRestCenter(const HexaID h) const;

    void getHexahedronVertexCoordinates(const HexaID h, Coord[8]) const;

    void getRestHexahedronVertexCoordinates(const HexaID h, Coord[8]) const;

    /// computes the volume of hexahedron no h and returns it
    Real computeHexahedronVolume(const HexaID h) const;

    /// computes the hexahedron volume of all hexahedra are store in the array interface
    void computeHexahedronVolume( BasicArrayInterface<Real> &ai) const;

    /// computes the hexahedron volume  of hexahedron no i and returns it
    Real computeRestHexahedronVolume(const HexaID h) const;

    /// computes barycentric coordinates corresponding to a given position. Warning: this method is only correct if the hexahedron is not deformed
    defaulttype::Vector3 computeHexahedronBarycentricCoeficients(const HexaID h, const Coord& p) const;

    defaulttype::Vector3 computeHexahedronRestBarycentricCoeficients(const HexaID h, const Coord& p) const;

    /// computes a position corresponding to given barycentric coordinates
    Coord getPointPositionInHexahedron(const HexaID h, const defaulttype::Vector3& baryC) const;

    Coord getRestPointPositionInHexahedron(const HexaID h, const defaulttype::Vector3& baryC) const;

    /// computes a position corresponding to given barycentric coordinates
    Coord getPointPositionInHexahedron(const HexaID h, const Real baryC[3]) const;

    Coord getRestPointPositionInHexahedron(const HexaID h, const Real baryC[3]) const;

    /// finds a hexahedron which is nearest to a given point. Computes barycentric coordinates and a distance measure.
    virtual int findNearestElement(const Coord& pos, defaulttype::Vector3& baryC, Real& distance) const;

    /// given a vector of points, find the nearest hexa for each point. Computes barycentric coordinates and a distance measure.
    virtual void findNearestElements(const VecCoord& pos, helper::vector<int>& elem,
            helper::vector<defaulttype::Vector3>& baryC, helper::vector<Real>& dist) const;

    /// If the point is inside the element, the distance measure is < 0. If the point is outside the element, the distance measure is a squared distance to the element center.
    virtual Real computeElementDistanceMeasure(const HexaID h, const Coord p) const;

    virtual Real computeElementRestDistanceMeasure(const HexaID h, const Coord p) const;

    /** \brief Write the current mesh into a msh file
    */
    void writeMSHfile(const char *filename) const;
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
