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
#ifndef SOFA_COMPONENT_TOPOLOGY_EDGESETTOPOLOGYCONTAINER_H
#define SOFA_COMPONENT_TOPOLOGY_EDGESETTOPOLOGYCONTAINER_H

#include <sofa/component/topology/PointSetTopologyContainer.h>

namespace sofa
{

namespace component
{

namespace topology
{
using core::componentmodel::topology::BaseMeshTopology;

typedef BaseMeshTopology::PointID			PointID;
typedef BaseMeshTopology::EdgeID			EdgeID;
typedef BaseMeshTopology::Edge				Edge;
typedef BaseMeshTopology::SeqEdges			SeqEdges;
typedef BaseMeshTopology::VertexEdges		VertexEdges;

/** a class that stores a set of edges  and provides access to the adjacency between points and edges */
class EdgeSetTopologyContainer : public PointSetTopologyContainer
{
    friend class EdgeSetTopologyModifier;

public:
    EdgeSetTopologyContainer();

    EdgeSetTopologyContainer(const sofa::helper::vector< Edge > &edges);

    virtual ~EdgeSetTopologyContainer() {}

    /// Procedural creation methods
    /// @{
    virtual void clear();
    virtual void addEdge( int a, int b );
    /// @}

    virtual void init();

    /// BaseMeshTopology API
    /// @{

    virtual const SeqEdges& getEdges()
    {
        return getEdgeArray();
    }

    /** \brief Returns the edge shell of the ith DOF.
    *
    */
    virtual const VertexEdges& getEdgeVertexShell(PointID i);

    /** \brief Returns the index of the edge joining vertex v1 and vertex v2; returns -1 if no edge exists
    *
    */
    virtual int getEdgeIndex(PointID v1, PointID v2);

    /// @}

    /** \brief Checks if the topology is coherent
    *
    * Check if the shell arrays are coherent
    */
    virtual bool checkTopology() const;

    /** \brief Returns the number of edges in this topology.
    *  The difference to getNbEdges() is that this method does not generate the edge array if it does not exist.
    */
    unsigned int getNumberOfEdges() const;

    /** \brief Returns the Edge array.
    *
    */
    virtual const sofa::helper::vector<Edge> &getEdgeArray();


    /** \brief Returns the Edge Shell array.
    *
    */
    virtual const sofa::helper::vector< sofa::helper::vector<unsigned int> > &getEdgeVertexShellArray();


    /** \brief Returns the number of connected components from the graph containing all edges and give, for each vertex, which component it belongs to  (use BOOST GRAPH LIBRAIRY)
    @param components the array containing the optimal vertex permutation according to the Reverse CuthillMckee algorithm
    */
    virtual int getNumberConnectedComponents(sofa::helper::vector<unsigned int>& components);

    inline friend std::ostream& operator<< (std::ostream& out, const EdgeSetTopologyContainer& t)
    {
        out << t.m_edge.size();
        for (unsigned int i=0; i<t.m_edge.size(); i++)
            out << " " << t.m_edge[i][0] << " " << t.m_edge[i][1] ;

        return out;
    }

    /// Needed to be compliant with Datas.
    inline friend std::istream& operator>>(std::istream& in, EdgeSetTopologyContainer& t)
    {
        unsigned int s;
        in >> s;
        for (unsigned int i=0; i<s; i++)
        {
            Edge T; in >> T;
            t.m_edge.push_back(T);
        }
        return in;
    }

    /** \brief Returns a non-const edge shell of the ith DOF for subsequent modification
    *
    */
    virtual sofa::helper::vector< unsigned int > &getEdgeVertexShellForModification(const unsigned int i);

    /** \brief Creates the EdgeSet array.
    *
    * This function must be implemented by derived classes to create a list of edges from a set of triangles or tetrahedra
    */
    virtual void createEdgeSetArray();

    /** \brief Creates the EdgeSetIndex.
    *
    * This function is only called if the EdgeShell member is required.
    * EdgeShell[i] contains the indices of all edges having the ith DOF as
    * one of their ends.
    */
    virtual void createEdgeVertexShellArray();

    bool hasEdges() const;

    bool hasEdgeVertexShell() const;

    void clearEdges();

    void clearEdgeVertexShell();

protected:
    /*** The array that stores the set of edges in the edge set */
    sofa::helper::vector<Edge> m_edge;

    /** the array that stores the set of edge-vertex shells, ie for each vertex gives the set of adjacent edges */
    sofa::helper::vector< sofa::helper::vector< unsigned int > > m_edgeVertexShell;

    virtual void loadFromMeshLoader(sofa::component::MeshLoader* loader);
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
