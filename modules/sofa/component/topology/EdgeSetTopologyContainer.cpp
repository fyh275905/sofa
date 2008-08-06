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
#include <sofa/component/topology/EdgeSetTopologyContainer.h>

#include <sofa/core/ObjectFactory.h>
// Use BOOST GRAPH LIBRARY :

#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <utility>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/graph/bandwidth.hpp>

#include <sofa/component/MeshLoader.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace std;
using namespace sofa::defaulttype;
SOFA_DECL_CLASS(EdgeSetTopologyContainer)
int EdgeSetTopologyContainerClass = core::RegisterObject("Edge set topology container")
        .add< EdgeSetTopologyContainer >()
        ;

EdgeSetTopologyContainer::EdgeSetTopologyContainer()
    : PointSetTopologyContainer( )
{}

EdgeSetTopologyContainer::EdgeSetTopologyContainer(const sofa::helper::vector< Edge > &edges )
    : PointSetTopologyContainer( )
    , m_edge( edges )
{
    for (unsigned int i=0; i<m_edge.size(); ++i)
    {
        for(unsigned int j=0; j<2; ++j)
        {
            int a = m_edge[i][j];
            if (a >= (int)nbPoints) nbPoints = a+1;
        }
    }
}

void EdgeSetTopologyContainer::init()
{
    PointSetTopologyContainer::init();
}

void EdgeSetTopologyContainer::loadFromMeshLoader(sofa::component::MeshLoader* loader)
{
    // load points
    PointSetTopologyContainer::loadFromMeshLoader(loader);
    m_edge = loader->getEdges();
}

void EdgeSetTopologyContainer::addEdge(int a, int b)
{
    m_edge.push_back(Edge(a,b));
    if (a >= (int)nbPoints) nbPoints = a+1;
    if (b >= (int)nbPoints) nbPoints = b+1;
}

void EdgeSetTopologyContainer::createEdgeVertexShellArray()
{
    if(!hasEdges())	// this method should only be called when edges exist
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::createEdgeVertexShellArray] edge array is empty." << endl;
#endif
        createEdgeSetArray();
    }

    if(hasEdgeVertexShell())
    {
        clearEdgeVertexShell();
    }

    m_edgeVertexShell.resize( getNbPoints() );

    for (unsigned int edge=0; edge<m_edge.size(); ++edge)
    {
        // adding edge in the edge shell of both points
        m_edgeVertexShell[ m_edge[edge][0] ].push_back(edge);
        m_edgeVertexShell[ m_edge[edge][1] ].push_back(edge);
    }
}

void EdgeSetTopologyContainer::createEdgeSetArray()
{
#ifndef NDEBUG
    cout << "Error. [EdgeSetTopologyContainer::createEdgeSetArray] This method must be implemented by a child topology." << endl;
#endif
}

const sofa::helper::vector<Edge> &EdgeSetTopologyContainer::getEdgeArray()
{
    if(!hasEdges() && getNbPoints()>0)
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::getEdgeArray] creating edge array." << endl;
#endif
        createEdgeSetArray();
    }

    return m_edge;
}

int EdgeSetTopologyContainer::getEdgeIndex(PointID v1, PointID v2)
{
    if(!hasEdges()) // this method should only be called when edges exist
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::getEdgeIndex] edge array is empty." << endl;
#endif
        createEdgeSetArray();
    }

    if(!hasEdgeVertexShell())
        createEdgeVertexShellArray();

    const sofa::helper::vector< unsigned int > &es1 = getEdgeVertexShell(v1) ;

    int result = -1;
    for(unsigned int i=0; (i < es1.size()) && (result == -1); ++i)
    {
        const Edge &e = m_edge[ es1[i] ];
        if ((e[0] == v2) || (e[1] == v2))
            result = (int) es1[i];
    }
    return result;
}

// Return the number of connected components from the graph containing all edges and give, for each vertex, which component it belongs to  (use BOOST GRAPH LIBRAIRY)
int EdgeSetTopologyContainer::getNumberConnectedComponents(sofa::helper::vector<unsigned int>& components)
{
    using namespace boost;

    typedef adjacency_list <vecS, vecS, undirectedS> Graph;

    Graph G;

    for (unsigned int k=0; k<m_edge.size(); ++k)
    {
        add_edge(m_edge[k][0], m_edge[k][1], G);
    }

    components.resize(num_vertices(G));
    int num = (int) connected_components(G, &components[0]);

    return num;
}

bool EdgeSetTopologyContainer::checkTopology() const
{
#ifndef NDEBUG
    bool ret = true;

    if(hasEdgeVertexShell())
    {
        for (unsigned int i=0; i<m_edgeVertexShell.size(); ++i)
        {
            const sofa::helper::vector<unsigned int> &es = m_edgeVertexShell[i];

            for (unsigned int j=0; j<es.size(); ++j)
            {
                bool check_edge_vertex_shell = (m_edge[ es[j] ][0] == i) ||  (m_edge[ es[j] ][1] == i);
                if(! check_edge_vertex_shell)
                {
                    std::cout << "*** CHECK FAILED : check_edge_vertex_shell, i = " << i << " , j = " << j << std::endl;
                    ret = false;
                }
            }
        }
    }

    return ret &&  PointSetTopologyContainer::checkTopology();
#else
    return true;
#endif
}

unsigned int EdgeSetTopologyContainer::getNumberOfEdges() const
{
    return m_edge.size();
}

const sofa::helper::vector< sofa::helper::vector<unsigned int> > &EdgeSetTopologyContainer::getEdgeVertexShellArray()
{
    if(!hasEdgeVertexShell())
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::getEdgeVertexShellArray] edge vertex shell array is empty." << endl;
#endif
        createEdgeVertexShellArray();
    }

    return m_edgeVertexShell;
}

const VertexEdges& EdgeSetTopologyContainer::getEdgeVertexShell(PointID i)
{
    if(!hasEdgeVertexShell())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::getEdgeVertexShell] edge vertex shell array is empty." << endl;
#endif
        createEdgeVertexShellArray();
    }

#ifndef NDEBUG
    if(m_edgeVertexShell.size() <= i)
        cout << "Error. [EdgeSetTopologyContainer::getEdgeVertexShell] edge vertex shell array out of bounds: "
                << i << " >= " << m_edgeVertexShell.size() << endl;
#endif

    return m_edgeVertexShell[i];
}

sofa::helper::vector< unsigned int > &EdgeSetTopologyContainer::getEdgeVertexShellForModification(const unsigned int i)
{
    if(!hasEdgeVertexShell())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        cout << "Warning. [EdgeSetTopologyContainer::getEdgeVertexShellForModification] edge vertex shell array is empty." << endl;
#endif
        createEdgeVertexShellArray();
    }

    return m_edgeVertexShell[i];
}

bool EdgeSetTopologyContainer::hasEdges() const
{
    return !m_edge.empty();
}

bool EdgeSetTopologyContainer::hasEdgeVertexShell() const
{
    return !m_edgeVertexShell.empty();
}

void EdgeSetTopologyContainer::clearEdges()
{
    m_edge.clear();
}

void EdgeSetTopologyContainer::clearEdgeVertexShell()
{
    m_edgeVertexShell.clear();
}

void EdgeSetTopologyContainer::clear()
{
    clearEdges();
    clearEdgeVertexShell();

    PointSetTopologyContainer::clear();
}

} // namespace topology

} // namespace component

} // namespace sofa

