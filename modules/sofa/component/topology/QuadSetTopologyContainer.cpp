/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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

#include <sofa/component/topology/QuadSetTopologyContainer.h>

#include <sofa/core/ObjectFactory.h>

#include <sofa/component/container/MeshLoader.h>

namespace sofa
{
namespace component
{
namespace topology
{

using namespace std;
using namespace sofa::defaulttype;

SOFA_DECL_CLASS(QuadSetTopologyContainer)
int QuadSetTopologyContainerClass = core::RegisterObject("Quad set topology container")
        .add< QuadSetTopologyContainer >()
        ;

QuadSetTopologyContainer::QuadSetTopologyContainer()
    : EdgeSetTopologyContainer()
    , d_quad(initData(&d_quad, "quads", "List of quad indices"))
{
}


void QuadSetTopologyContainer::addQuad( int a, int b, int c, int d )
{
    helper::WriteAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;
    m_quad.push_back(Quad(a,b,c,d));
    if (a >= getNbPoints()) nbPoints.setValue(a+1);
    if (b >= getNbPoints()) nbPoints.setValue(b+1);
    if (c >= getNbPoints()) nbPoints.setValue(c+1);
    if (d >= getNbPoints()) nbPoints.setValue(d+1);
}

void QuadSetTopologyContainer::init()
{
    EdgeSetTopologyContainer::init();
    d_quad.updateIfDirty(); // make sure m_quad is up to date
}

void QuadSetTopologyContainer::loadFromMeshLoader(sofa::component::container::MeshLoader* loader)
{
    // load points
    helper::ReadAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;
    if (!m_quad.empty()) return;
    PointSetTopologyContainer::loadFromMeshLoader(loader);
    loader->getQuads(*(d_quad.beginEdit()));
    d_quad.endEdit();
}

void QuadSetTopologyContainer::createQuadSetArray()
{
#ifndef NDEBUG
    sout << "Error. [QuadSetTopologyContainer::createQuadSetArray] This method must be implemented by a child topology." << endl;
#endif
}

void QuadSetTopologyContainer::createQuadsAroundVertexArray()
{
    if(!hasQuads()) // this method should only be called when quads exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createQuadsAroundVertexArray] quad array is empty." << endl;
#endif
        createQuadSetArray();
    }

    if(hasQuadsAroundVertex())
    {
        clearQuadsAroundVertex();
    }

    m_quadsAroundVertex.resize( getNbPoints() );
    helper::ReadAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;

    for (unsigned int i=0; i<m_quad.size(); ++i)
    {
        // adding quad i in the quad shell of all points
        for (unsigned int j=0; j<4; ++j)
        {
            m_quadsAroundVertex[ m_quad[i][j] ].push_back( i );
        }
    }
}

void QuadSetTopologyContainer::createQuadsAroundEdgeArray()
{
    if(!hasQuads()) // this method should only be called when quads exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createQuadsAroundEdgeArray] quad array is empty." << endl;
#endif
        createQuadSetArray();
    }

    if(!hasEdges()) // this method should only be called when edges exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createQuadsAroundEdgeArray] edge array is empty." << endl;
#endif
        createEdgeSetArray();
    }

    if(!hasEdgesInQuad())
        createEdgesInQuadArray();

    const unsigned int numQuads = getNumberOfQuads();
    const unsigned int numEdges = getNumberOfEdges();

    if(hasQuadsAroundEdge())
    {
        clearQuadsAroundEdge();
    }

    m_quadsAroundEdge.resize(numEdges);

    for (unsigned int i=0; i<numQuads; ++i)
    {
        // adding quad i in the quad shell of all edges
        for (unsigned int j=0; j<4; ++j)
        {
            m_quadsAroundEdge[ m_edgesInQuad[i][j] ].push_back( i );
        }
    }
}

void QuadSetTopologyContainer::createEdgeSetArray()
{
    if(!hasQuads()) // this method should only be called when quads exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createEdgeSetArray] quad array is empty." << endl;
#endif
        createQuadSetArray();
    }

    if(hasEdges())
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createEdgeSetArray] edge array is not empty." << endl;
#endif

        // clear edges and all shells that depend on edges
        EdgeSetTopologyContainer::clear();

        if(hasEdgesInQuad())
            clearEdgesInQuad();

        if(hasQuadsAroundEdge())
            clearQuadsAroundEdge();
    }

    // create a temporary map to find redundant edges
    std::map<Edge, unsigned int> edgeMap;
    helper::WriteAccessor< Data< sofa::helper::vector<Edge> > > m_edge = d_edge;
    helper::ReadAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;

    for (unsigned int i=0; i<m_quad.size(); ++i)
    {
        const Quad &t = m_quad[i];
        for(unsigned int j=0; j<4; ++j)
        {
            const unsigned int v1 = t[(j+1)%4];
            const unsigned int v2 = t[(j+2)%4];

            // sort vertices in lexicographic order
            const Edge e = ((v1<v2) ? Edge(v1,v2) : Edge(v2,v1));

            if(edgeMap.find(e) == edgeMap.end())
            {
                // edge not in edgeMap so create a new one
                const int edgeIndex = edgeMap.size();
                edgeMap[e] = edgeIndex;
                m_edge.push_back(e);
            }
        }
    }
}

void QuadSetTopologyContainer::createEdgesInQuadArray()
{
    if(!hasQuads()) // this method should only be called when quads exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createEdgesInQuadArray] quad array is empty." << endl;
#endif
        createQuadSetArray();
    }

    if(!hasEdges()) // this method should only be called when edges exist
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::createEdgesInQuadArray] edge array is empty." << endl;
#endif
        createEdgeSetArray();
    }

    if(hasEdgesInQuad())
        clearEdgesInQuad();

    const unsigned int numQuads = getNumberOfQuads();

    m_edgesInQuad.resize( numQuads );
    helper::ReadAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;

    for(unsigned int i=0; i<numQuads; ++i)
    {
        const Quad &t = m_quad[i];
        // adding edge i in the edge shell of both points
        for (unsigned int j=0; j<4; ++j)
        {
            const int edgeIndex = getEdgeIndex(t[(j+1)%4],t[(j+2)%4]);
            m_edgesInQuad[i][j]=edgeIndex;
        }
    }
}

const sofa::helper::vector<Quad> &QuadSetTopologyContainer::getQuadArray()
{
    if(!hasQuads() && getNbPoints()>0)
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadArray] creating quad array." << endl;
#endif
        createQuadSetArray();
    }

    return d_quad.getValue();
}

const Quad QuadSetTopologyContainer::getQuad (QuadID i)
{
    if(!hasQuads())
        createQuadSetArray();

    return (d_quad.getValue())[i];
}


int QuadSetTopologyContainer::getQuadIndex(PointID v1, PointID v2, PointID v3, PointID v4)
{
    if(!hasQuadsAroundVertex())
        createQuadsAroundVertexArray();

    sofa::helper::vector<unsigned int> set1 = getQuadsAroundVertex(v1);
    sofa::helper::vector<unsigned int> set2 = getQuadsAroundVertex(v2);
    sofa::helper::vector<unsigned int> set3 = getQuadsAroundVertex(v3);
    sofa::helper::vector<unsigned int> set4 = getQuadsAroundVertex(v4);

    sort(set1.begin(), set1.end());
    sort(set2.begin(), set2.end());
    sort(set3.begin(), set3.end());
    sort(set4.begin(), set4.end());

    // The destination vector must be large enough to contain the result.
    sofa::helper::vector<unsigned int> out1(set1.size()+set2.size());
    sofa::helper::vector<unsigned int>::iterator result1;
    result1 = std::set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),out1.begin());
    out1.erase(result1,out1.end());

    sofa::helper::vector<unsigned int> out2(set3.size()+out1.size());
    sofa::helper::vector<unsigned int>::iterator result2;
    result2 = std::set_intersection(set3.begin(),set3.end(),out1.begin(),out1.end(),out2.begin());
    out2.erase(result2,out2.end());

    sofa::helper::vector<unsigned int> out3(set4.size()+out2.size());
    sofa::helper::vector<unsigned int>::iterator result3;
    result3 = std::set_intersection(set4.begin(),set4.end(),out2.begin(),out2.end(),out3.begin());
    out3.erase(result3,out3.end());

#ifndef NDEBUG
    if(out3.size() > 1)
        sout << "Warning. [QuadSetTopologyContainer::getQuadIndex] more than one quad found" << endl;
#endif

    if(out3.size()==1)
        return (int) (out3[0]);
    else
        return -1;
}

unsigned int QuadSetTopologyContainer::getNumberOfQuads() const
{
    helper::ReadAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;
    return m_quad.size();
}

const sofa::helper::vector< sofa::helper::vector<unsigned int> > &QuadSetTopologyContainer::getQuadsAroundVertexArray()
{
    if(!hasQuadsAroundVertex())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundVertexArray] quad vertex shell array is empty." << endl;
#endif
        createQuadsAroundVertexArray();
    }

    return m_quadsAroundVertex;
}

const sofa::helper::vector< sofa::helper::vector<unsigned int> > &QuadSetTopologyContainer::getQuadsAroundEdgeArray()
{
    if(!hasQuadsAroundEdge())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundEdgeArray] quad edge shell array is empty." << endl;
#endif
        createQuadsAroundEdgeArray();
    }

    return m_quadsAroundEdge;
}

const sofa::helper::vector< EdgesInQuad> &QuadSetTopologyContainer::getEdgesInQuadArray()
{
    if(m_edgesInQuad.empty())
        createEdgesInQuadArray();

    return m_edgesInQuad;
}

const QuadsAroundVertex& QuadSetTopologyContainer::getQuadsAroundVertex(PointID i)
{
    if(!hasQuadsAroundVertex())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundVertex] quad vertex shell array is empty." << endl;
#endif
        createQuadsAroundVertexArray();
    }
    else if( i >= m_quadsAroundVertex.size())
    {
#ifndef NDEBUG
        sout << "Error. [QuadSetTopologyContainer::getQuadsAroundVertex] index out of bounds." << endl;
#endif
        createQuadsAroundVertexArray();
    }

    return m_quadsAroundVertex[i];
}

const QuadsAroundEdge& QuadSetTopologyContainer::getQuadsAroundEdge(EdgeID i)
{
    if(!hasQuadsAroundEdge())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundEdge] quad edge shell array is empty." << endl;
#endif
        createQuadsAroundEdgeArray();
    }
    else if( i >= m_quadsAroundEdge.size())
    {
#ifndef NDEBUG
        sout << "Error. [QuadSetTopologyContainer::getQuadsAroundEdge] index out of bounds." << endl;
#endif
        createQuadsAroundEdgeArray();
    }

    return m_quadsAroundEdge[i];
}

const EdgesInQuad &QuadSetTopologyContainer::getEdgesInQuad(const unsigned int i)
{
    if(m_edgesInQuad.empty())
        createEdgesInQuadArray();

    if( i >= m_edgesInQuad.size())
    {
#ifndef NDEBUG
        sout << "Error. [QuadSetTopologyContainer::getEdgesInQuad] index out of bounds." << endl;
#endif
        createEdgesInQuadArray();
    }

    return m_edgesInQuad[i];
}

int QuadSetTopologyContainer::getVertexIndexInQuad(const Quad &t, unsigned int vertexIndex) const
{
    if(t[0]==vertexIndex)
        return 0;
    else if(t[1]==vertexIndex)
        return 1;
    else if(t[2]==vertexIndex)
        return 2;
    else if(t[3]==vertexIndex)
        return 3;
    else
        return -1;
}

int QuadSetTopologyContainer::getEdgeIndexInQuad(const EdgesInQuad &t, unsigned int edgeIndex) const
{
    if(t[0]==edgeIndex)
        return 0;
    else if(t[1]==edgeIndex)
        return 1;
    else if(t[2]==edgeIndex)
        return 2;
    else if(t[3]==edgeIndex)
        return 3;
    else
        return -1;
}

sofa::helper::vector< unsigned int > &QuadSetTopologyContainer::getQuadsAroundEdgeForModification(const unsigned int i)
{
    if(!hasQuadsAroundEdge())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundEdgeForModification] quad edge shell array is empty." << endl;
#endif
        createQuadsAroundEdgeArray();
    }

    if( i >= m_quadsAroundEdge.size())
    {
#ifndef NDEBUG
        sout << "Error. [QuadSetTopologyContainer::getQuadsAroundEdgeForModification] index out of bounds." << endl;
#endif
        createQuadsAroundEdgeArray();
    }

    return m_quadsAroundEdge[i];
}

sofa::helper::vector< unsigned int > &QuadSetTopologyContainer::getQuadsAroundVertexForModification(const unsigned int i)
{
    if(!hasQuadsAroundVertex())	// this method should only be called when the shell array exists
    {
#ifndef NDEBUG
        sout << "Warning. [QuadSetTopologyContainer::getQuadsAroundVertexForModification] quad vertex shell array is empty." << endl;
#endif
        createQuadsAroundVertexArray();
    }

    if( i >= m_quadsAroundVertex.size())
    {
#ifndef NDEBUG
        sout << "Error. [QuadSetTopologyContainer::getQuadsAroundVertexForModification] index out of bounds." << endl;
#endif
        createQuadsAroundVertexArray();
    }

    return m_quadsAroundVertex[i];
}


bool QuadSetTopologyContainer::checkTopology() const
{
#ifndef NDEBUG
    bool ret = true;

    if(hasQuadsAroundVertex())
    {
        for (unsigned int i=0; i<m_quadsAroundVertex.size(); ++i)
        {
            const sofa::helper::vector<unsigned int> &tvs = m_quadsAroundVertex[i];
            for (unsigned int j=0; j<tvs.size(); ++j)
            {
                if((m_quad[tvs[j]][0]!=i)
                   && (m_quad[tvs[j]][1]!=i)
                   && (m_quad[tvs[j]][2]!=i)
                   && (m_quad[tvs[j]][3]!=i))
                {
                    ret = false;
                    std::cout << "*** CHECK FAILED : check_quad_vertex_shell, i = " << i << " , j = " << j << std::endl;
                }
            }
        }
    }

    if(hasQuadsAroundEdge())
    {
        for (unsigned int i=0; i<m_quadsAroundEdge.size(); ++i)
        {
            const sofa::helper::vector<unsigned int> &tes = m_quadsAroundEdge[i];
            for (unsigned int j=0; j<tes.size(); ++j)
            {
                if((m_edgesInQuad[tes[j]][0]!=i)
                   && (m_edgesInQuad[tes[j]][1]!=i)
                   && (m_edgesInQuad[tes[j]][2]!=i)
                   && (m_edgesInQuad[tes[j]][3]!=i))
                {
                    ret = false;
                    std::cout << "*** CHECK FAILED : check_quad_edge_shell, i = " << i << " , j = " << j << std::endl;
                }
            }
        }
    }

    return ret && EdgeSetTopologyContainer::checkTopology();
#else
    return true;
#endif
}

bool QuadSetTopologyContainer::hasQuads() const
{
    d_quad.updateIfDirty();
    return !(d_quad.getValue()).empty();
}

bool QuadSetTopologyContainer::hasEdgesInQuad() const
{
    return !m_edgesInQuad.empty();
}

bool QuadSetTopologyContainer::hasQuadsAroundVertex() const
{
    return !m_quadsAroundVertex.empty();
}

bool QuadSetTopologyContainer::hasQuadsAroundEdge() const
{
    return !m_quadsAroundEdge.empty();
}

void QuadSetTopologyContainer::clearQuadsAroundVertex()
{
    for(unsigned int i=0; i<m_quadsAroundVertex.size(); ++i)
        m_quadsAroundVertex[i].clear();

    m_quadsAroundVertex.clear();
}

void QuadSetTopologyContainer::clearQuadsAroundEdge()
{
    for(unsigned int i=0; i<m_quadsAroundEdge.size(); ++i)
        m_quadsAroundEdge[i].clear();

    m_quadsAroundEdge.clear();
}

void QuadSetTopologyContainer::clearEdgesInQuad()
{
    m_edgesInQuad.clear();
}

void QuadSetTopologyContainer::clearQuads()
{
    helper::WriteAccessor< Data< sofa::helper::vector<Quad> > > m_quad = d_quad;
    m_quad.clear();
}

void QuadSetTopologyContainer::clear()
{
    clearQuadsAroundVertex();
    clearQuadsAroundEdge();
    clearEdgesInQuad();
    clearQuads();

    EdgeSetTopologyContainer::clear();
}

} // namespace topology

} // namespace component

} // namespace sofa

