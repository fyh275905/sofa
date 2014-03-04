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
#ifndef SOFA_COMPONENT_TOPOLOGY_MESH2BEZIERTOPOLOGICALMAPPING_H
#define SOFA_COMPONENT_TOPOLOGY_MESH2BEZIERTOPOLOGICALMAPPING_H


#include <TopologyMapping/topology/Mesh2PointTopologicalMapping.h>



namespace sofa
{
namespace component
{
namespace topology
{
using namespace sofa::defaulttype;
using namespace sofa::component::topology;
using namespace sofa::core::topology;
using namespace sofa::core;

/**
 * This class, called Mesh2BezierTopologicalMapping, is a specific implementation of the interface TopologicalMapping where :
 *
 * INPUT TOPOLOGY = any Tetrahedral MeshTopology
 * OUTPUT TOPOLOGY = A BezierTetrahedralPointSetTopology as a tesselated version of the input mesh 
 *
 * This Topological mapping is a specific implementation of the Mesh2PointTopologicalMapping with a small overhead
 *
 * Mesh2BezierTopologicalMapping class is templated by the pair (INPUT TOPOLOGY, OUTPUT TOPOLOGY)
 *
*/

class SOFA_TOPOLOGY_MAPPING_API Mesh2BezierTopologicalMapping : public Mesh2PointTopologicalMapping
{
public:
    SOFA_CLASS(Mesh2BezierTopologicalMapping,Mesh2PointTopologicalMapping);
protected:
    /** \brief Constructor.
     *
     */
    Mesh2BezierTopologicalMapping ();

    /** \brief Destructor.
     *
         * Does nothing.
         */
    virtual ~Mesh2BezierTopologicalMapping() {};
public:
    /** \brief Initializes the target BaseTopology from the source BaseTopology.
     */
    virtual void init();
protected:
    /// Fills pointBaryCoords, edgeBaryCoords, triangleBaryCoords and tetraBaryCoords so as to create a Bezier Tetrahedron mesh of a given order
	Data < unsigned int > bezierTetrahedronDegree;
};

} // namespace topology
} // namespace component
} // namespace sofa

#endif // SOFA_COMPONENT_TOPOLOGY_MESH2BEZIERTOPOLOGICALMAPPING_H
