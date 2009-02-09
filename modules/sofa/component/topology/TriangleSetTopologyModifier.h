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
#ifndef SOFA_COMPONENT_TOPOLOGY_TRIANGLESETTOPOLOGYMODIFIER_H
#define SOFA_COMPONENT_TOPOLOGY_TRIANGLESETTOPOLOGYMODIFIER_H

#include <sofa/component/topology/EdgeSetTopologyModifier.h>

namespace sofa
{
namespace component
{
namespace topology
{
class TriangleSetTopologyContainer;

using core::componentmodel::topology::BaseMeshTopology;
typedef BaseMeshTopology::TriangleID TriangleID;
typedef BaseMeshTopology::Triangle Triangle;
typedef BaseMeshTopology::SeqTriangles SeqTriangles;
typedef BaseMeshTopology::VertexTriangles VertexTriangles;
typedef BaseMeshTopology::EdgeTriangles EdgeTriangles;
typedef BaseMeshTopology::TriangleEdges TriangleEdges;

/**
 * A class that modifies the topology by adding and removing triangles
 */
class SOFA_COMPONENT_CONTAINER_API TriangleSetTopologyModifier : public EdgeSetTopologyModifier
{
public:
    TriangleSetTopologyModifier()
        : EdgeSetTopologyModifier()
    { }

    virtual ~TriangleSetTopologyModifier() {}

    virtual void init();

    /** \brief Sends a message to warn that some triangles were added in this topology.
     *
     * \sa addTrianglesProcess
     */
    void addTrianglesWarning(const unsigned int nTriangles,
            const sofa::helper::vector< Triangle >& trianglesList,
            const sofa::helper::vector< unsigned int >& trianglesIndexList) ;

    /** \brief Sends a message to warn that some triangles were added in this topology.
     *
     * \sa addTrianglesProcess
     */
    void addTrianglesWarning(const unsigned int nTriangles,
            const sofa::helper::vector< Triangle >& trianglesList,
            const sofa::helper::vector< unsigned int >& trianglesIndexList,
            const sofa::helper::vector< sofa::helper::vector< unsigned int > > & ancestors,
            const sofa::helper::vector< sofa::helper::vector< double > >& baryCoefs) ;

    /** \brief Add a triangle.
     */
    void addTriangleProcess(Triangle e);

    /** \brief Actually Add some triangles to this topology.
     *
     * \sa addTrianglesWarning
     */
    virtual void addTrianglesProcess(const sofa::helper::vector< Triangle > &triangles);

    /** \brief Add some points to this topology.
     *
     * \sa addPointsWarning
     */
    virtual void addPointsProcess(const unsigned int nPoints);

    /** \brief Sends a message to warn that some edges were added in this topology.
     *
     * \sa addEdgesProcess
     */
    void addEdgesWarning(const unsigned int nEdges,
            const sofa::helper::vector< Edge >& edgesList,
            const sofa::helper::vector< unsigned int >& edgesIndexList)
    {
        EdgeSetTopologyModifier::addEdgesWarning( nEdges, edgesList, edgesIndexList);
    }

    /** \brief Sends a message to warn that some edges were added in this topology.
     *
     * \sa addEdgesProcess
     */
    void addEdgesWarning(const unsigned int nEdges,
            const sofa::helper::vector< Edge >& edgesList,
            const sofa::helper::vector< unsigned int >& edgesIndexList,
            const sofa::helper::vector< sofa::helper::vector< unsigned int > > & ancestors,
            const sofa::helper::vector< sofa::helper::vector< double > >& baryCoefs)
    {
        EdgeSetTopologyModifier::addEdgesWarning( nEdges, edgesList, edgesIndexList, ancestors, baryCoefs);
    }

    /** \brief Add some edges to this topology.
     *
     * \sa addEdgesWarning
     */
    void addEdgesProcess(const sofa::helper::vector< Edge > &edges);





    /** \brief Generic method to remove a list of items.
     */
    virtual void removeItems(sofa::helper::vector< unsigned int >& items);

    /** \brief Remove a set  of triangles
        @param triangles an array of triangle indices to be removed (note that the array is not const since it needs to be sorted)
        *
        @param removeIsolatedEdges if true isolated edges are also removed
        @param removeIsolatedPoints if true isolated vertices are also removed
        *
        */
    virtual void removeTriangles(sofa::helper::vector< unsigned int >& triangles,
            const bool removeIsolatedEdges,
            const bool removeIsolatedPoints);


    /** \brief Sends a message to warn that some triangles are about to be deleted.
     *
     * \sa removeTrianglesProcess
     *
     * Important : parameter indices is not const because it is actually sorted from the highest index to the lowest one.
     */
    virtual void removeTrianglesWarning( sofa::helper::vector<unsigned int> &triangles);


    /** \brief Remove a subset of  triangles. Eventually remove isolated edges and vertices
     *
     * Important : some structures might need to be warned BEFORE the points are actually deleted, so always use method removeEdgesWarning before calling removeEdgesProcess.
     * \sa removeTrianglesWarning
     *
     * @param removeIsolatedEdges if true isolated edges are also removed
     * @param removeIsolatedPoints if true isolated vertices are also removed
     */
    virtual void removeTrianglesProcess( const sofa::helper::vector<unsigned int> &indices,
            const bool removeIsolatedEdges=false,
            const bool removeIsolatedPoints=false);





    /** \brief Remove a subset of edges
     *
     * Important : some structures might need to be warned BEFORE the points are actually deleted, so always use method removeEdgesWarning before calling removeEdgesProcess.
     * \sa removeEdgesWarning
     *
     * @param removeIsolatedItems if true isolated vertices are also removed
     * Important : parameter indices is not const because it is actually sorted from the highest index to the lowest one.
     */
    virtual void removeEdgesProcess( const sofa::helper::vector<unsigned int> &indices,
            const bool removeIsolatedItems=false);


    /** \brief Remove a subset of points
     *
     * Elements corresponding to these points are removed from the mechanical object's state vectors.
     *
     * Important : some structures might need to be warned BEFORE the points are actually deleted, so always use method removePointsWarning before calling removePointsProcess.
     * \sa removePointsWarning
     * Important : the points are actually deleted from the mechanical object's state vectors iff (removeDOF == true)
     */
    virtual void removePointsProcess(sofa::helper::vector<unsigned int> &indices,
            const bool removeDOF = true);









    /** \brief Reorder this topology.
     *
     * Important : the points are actually renumbered in the mechanical object's state vectors iff (renumberDOF == true)
     * \see MechanicalObject::renumberValues
     */
    virtual void renumberPointsProcess( const sofa::helper::vector<unsigned int> &index,
            const sofa::helper::vector<unsigned int> &inv_index,
            const bool renumberDOF = true);



    /** \brief Generic method for points renumbering
     */
    virtual void renumberPoints( const sofa::helper::vector<unsigned int> &index,
            const sofa::helper::vector<unsigned int> &inv_index);

    /** \brief Precondition to fulfill before removing triangles. No preconditions are needed in this class. This function should be inplemented in children classes.
    *
     */
    virtual bool removePrecondition(sofa::helper::vector< unsigned int >& items);

    /**\brief: Postprocessing to apply to the triangle topology. Nothing is needed in this class. This function should be inplemented in children classes.
    *
     */
    virtual void removePostProcessing(const sofa::helper::vector< unsigned int >& edgeToBeRemoved, const sofa::helper::vector< unsigned int >& vertexToBeRemoved )
    {(void)vertexToBeRemoved; (void)edgeToBeRemoved;};


    virtual void Debug() {}; // TO BE REMOVED WHEN MANIFOLD MODIFIER IS SURE.


private:
    TriangleSetTopologyContainer*	m_container;
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
