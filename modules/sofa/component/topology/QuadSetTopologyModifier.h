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
#ifndef SOFA_COMPONENT_TOPOLOGY_QUADSETTOPOLOGYMODIFIER_H
#define SOFA_COMPONENT_TOPOLOGY_QUADSETTOPOLOGYMODIFIER_H

#include <sofa/component/topology/EdgeSetTopologyModifier.h>

namespace sofa
{

namespace component
{

namespace topology
{
class QuadSetTopologyContainer;

using core::componentmodel::topology::BaseMeshTopology;
typedef BaseMeshTopology::QuadID QuadID;
typedef BaseMeshTopology::Quad Quad;
typedef BaseMeshTopology::SeqQuads SeqQuads;
typedef BaseMeshTopology::VertexQuads VertexQuads;
typedef BaseMeshTopology::EdgeQuads EdgeQuads;
typedef BaseMeshTopology::QuadEdges QuadEdges;

/**
* A class that modifies the topology by adding and removing quads
*/
class QuadSetTopologyModifier : public EdgeSetTopologyModifier
{
public:
    QuadSetTopologyModifier()
        : EdgeSetTopologyModifier()
    { }

    virtual ~QuadSetTopologyModifier() {}

    virtual void init();

    /** \brief Sends a message to warn that some quads were added in this topology.
    *
    * \sa addQuadsProcess
    */
    void addQuadsWarning(const unsigned int nQuads,
            const sofa::helper::vector< Quad >& quadsList,
            const sofa::helper::vector< unsigned int >& quadsIndexList);

    /** \brief Sends a message to warn that some quads were added in this topology.
    *
    * \sa addQuadsProcess
    */
    void addQuadsWarning(const unsigned int nQuads,
            const sofa::helper::vector< Quad >& quadsList,
            const sofa::helper::vector< unsigned int >& quadsIndexList,
            const sofa::helper::vector< sofa::helper::vector< unsigned int > > & ancestors,
            const sofa::helper::vector< sofa::helper::vector< double > >& baryCoefs);

    /** \brief Add a quad.
    */
    void addQuadProcess(Quad e);

    /** \brief Actually Add some quads to this topology.
    *
    * \sa addQuadsWarning
    */
    virtual void addQuadsProcess(const sofa::helper::vector< Quad > &quads);

    /** \brief Sends a message to warn that some quads are about to be deleted.
    *
    * \sa removeQuadsProcess
    *
    * Important : parameter indices is not const because it is actually sorted from the highest index to the lowest one.
    */
    virtual void removeQuadsWarning( sofa::helper::vector<unsigned int> &quads);

    /** \brief Remove a subset of  quads. Eventually remove isolated edges and vertices
    *
    * Important : some structures might need to be warned BEFORE the points are actually deleted, so always use method removeEdgesWarning before calling removeEdgesProcess.
    * \sa removeQuadsWarning
    *
    * @param removeIsolatedEdges if true isolated edges are also removed
    * @param removeIsolatedPoints if true isolated vertices are also removed
    */
    virtual void removeQuadsProcess( const sofa::helper::vector<unsigned int> &indices,
            const bool removeIsolatedEdges=false,
            const bool removeIsolatedPoints=false);

    /** \brief Add some edges to this topology.
    *
    * \sa addEdgesWarning
    */
    void addEdgesProcess(const sofa::helper::vector< Edge > &edges);

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

    /** \brief Add some points to this topology.
    *
    * \sa addPointsWarning
    */
    virtual void addPointsProcess(const unsigned int nPoints);

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
    virtual void renumberPointsProcess( const sofa::helper::vector<unsigned int>& index,
            const sofa::helper::vector<unsigned int>& inv_index,
            const bool renumberDOF = true);

    /** \brief Remove a set  of quads
    @param quads an array of quad indices to be removed (note that the array is not const since it needs to be sorted)
    *
    @param removeIsolatedEdges if true isolated edges are also removed
    @param removeIsolatedPoints if true isolated vertices are also removed
    *
    */
    virtual void removeQuads(sofa::helper::vector< unsigned int >& quads,
            const bool removeIsolatedEdges,
            const bool removeIsolatedPoints);

    /** \brief Generic method to remove a list of items.
    */
    virtual void removeItems(sofa::helper::vector< unsigned int >& items);

    /** \brief Generic method for points renumbering
    */
    virtual void renumberPoints( const sofa::helper::vector<unsigned int>& index,
            const sofa::helper::vector<unsigned int>& inv_index);


private:
    QuadSetTopologyContainer* 	m_container;
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
