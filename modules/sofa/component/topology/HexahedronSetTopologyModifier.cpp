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
#include <sofa/component/topology/HexahedronSetTopologyModifier.h>
#include <sofa/component/topology/HexahedronSetTopologyChange.h>
#include <sofa/component/topology/HexahedronSetTopologyContainer.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{
namespace component
{
namespace topology
{
SOFA_DECL_CLASS(HexahedronSetTopologyModifier)
int HexahedronSetTopologyModifierClass = core::RegisterObject("Hexahedron set topology modifier")
        .add< HexahedronSetTopologyModifier >();

using namespace std;
using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::behavior;

const unsigned int hexahedronEdgeArray[12][2]= {{0,1},{0,3},{0,4},{1,2},{1,5},{2,3},{2,6},{3,7},{4,5},{4,7},{5,6},{6,7}};


void HexahedronSetTopologyModifier::init()
{
    QuadSetTopologyModifier::init();
    this->getContext()->get(m_container);
}


void HexahedronSetTopologyModifier::addHexahedronProcess(Hexahedron t)
{
#ifndef NDEBUG
    // check if the 8 vertices are different
    assert(t[0]!=t[1]); assert(t[0]!=t[2]); assert(t[0]!=t[3]); assert(t[0]!=t[4]); assert(t[0]!=t[5]); assert(t[0]!=t[6]); assert(t[0]!=t[7]);
    assert(t[1]!=t[2]); assert(t[1]!=t[3]); assert(t[1]!=t[4]); assert(t[1]!=t[5]); assert(t[1]!=t[6]); assert(t[1]!=t[7]);
    assert(t[2]!=t[3]); assert(t[2]!=t[4]); assert(t[2]!=t[5]); assert(t[2]!=t[6]); assert(t[2]!=t[7]);
    assert(t[3]!=t[4]); assert(t[3]!=t[5]); assert(t[3]!=t[6]); assert(t[3]!=t[7]);
    assert(t[4]!=t[5]); assert(t[4]!=t[6]); assert(t[4]!=t[7]);
    assert(t[5]!=t[6]); assert(t[5]!=t[7]);
    assert(t[6]!=t[7]);

    // check if there already exists a hexahedron with the same indices
    assert(m_container->getHexahedronIndex(t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7])== -1);
#endif
    const unsigned int hexahedronIndex = m_container->getNumberOfHexahedra();

    if(m_container->hasHexahedronQuads())
    {
        int quadIndex;

        // Quad 0 :
        quadIndex=m_container->getQuadIndex(t[0],t[3],t[2],t[1]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[0],t[3],t[2],t[1]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[0],t[3],t[2],t[1]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][0]=quadIndex;

        // Quad 1 :
        quadIndex=m_container->getQuadIndex(t[4],t[5],t[6],t[7]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[4],t[5],t[6],t[7]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[4],t[5],t[6],t[7]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][1]=quadIndex;

        // Quad 2 :
        quadIndex=m_container->getQuadIndex(t[0],t[1],t[5],t[4]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[0],t[1],t[5],t[4]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[0],t[1],t[5],t[4]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][2]=quadIndex;

        // Quad 3 :
        quadIndex=m_container->getQuadIndex(t[1],t[2],t[6],t[5]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[1],t[2],t[6],t[5]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[1],t[2],t[6],t[5]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][3]=quadIndex;

        // Quad 4 :
        quadIndex=m_container->getQuadIndex(t[2],t[3],t[7],t[6]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[2],t[3],t[7],t[6]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[2],t[3],t[7],t[6]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][4]=quadIndex;

        // Quad 5 :
        quadIndex=m_container->getQuadIndex(t[3],t[0],t[4],t[7]);
        //assert(quadIndex!= -1);
        if(quadIndex == -1)
        {
            // first create the quad
            sofa::helper::vector< Quad > v;
            Quad e1 (t[3],t[0],t[4],t[7]);
            v.push_back(e1);

            addQuadsProcess((const sofa::helper::vector< Quad > &) v);

            quadIndex=m_container->getQuadIndex(t[3],t[0],t[4],t[7]);
            sofa::helper::vector< unsigned int > quadIndexList;
            quadIndexList.push_back(quadIndex);
            addQuadsWarning( v.size(), v,quadIndexList);
        }
        m_container->m_hexahedronQuad.resize(hexahedronIndex+1);
        m_container->m_hexahedronQuad[hexahedronIndex][5]=quadIndex;

        if(m_container->hasHexahedronQuadShell())
        {
            for(unsigned int q=0; q<6; ++q)
            {
                sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronQuadShell[m_container->m_hexahedronQuad[hexahedronIndex][q]];
                shell.push_back( hexahedronIndex );
            }
        }
    } // quads

    if(m_container->hasHexahedronEdges())
    {
        m_container->m_hexahedronEdge.resize(hexahedronIndex+1);
        for(unsigned int edgeIdx=0; edgeIdx<12; ++edgeIdx)
        {
            const int edgeIndex=m_container->getEdgeIndex(t[hexahedronEdgeArray[edgeIdx][0]],
                    t[hexahedronEdgeArray[edgeIdx][1]]);
            assert(edgeIndex!= -1);
            m_container->m_hexahedronEdge[hexahedronIndex][edgeIdx]= edgeIndex;
        }

        if(m_container->hasHexahedronEdgeShell())
        {
            for(unsigned int e=0; e<12; ++e)
            {
                sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronEdgeShell[m_container->m_hexahedronEdge[hexahedronIndex][e]];
                shell.push_back( hexahedronIndex );
            }
        }
    } // edges

    if(m_container->hasHexahedronVertexShell())
    {
        for(unsigned int v=0; v<8; ++v)
        {
            sofa::helper::vector< unsigned int > &shell = m_container->getHexahedronVertexShellForModification( t[v] );
            shell.push_back( hexahedronIndex );
        }
    }

    m_container->m_hexahedron.push_back(t);
}


void HexahedronSetTopologyModifier::addHexahedraProcess(const sofa::helper::vector< Hexahedron > &hexahedra)
{
    for(unsigned int i = 0; i < hexahedra.size(); ++i)
    {
        addHexahedronProcess(hexahedra[i]);
    }
}


void HexahedronSetTopologyModifier::addHexahedraWarning(const unsigned int nHexahedra,
        const sofa::helper::vector< Hexahedron >& hexahedraList,
        const sofa::helper::vector< unsigned int >& hexahedraIndexList)
{
    // Warning that hexahedra just got created
    HexahedraAdded *e = new HexahedraAdded(nHexahedra, hexahedraList, hexahedraIndexList);
    addTopologyChange(e);
}


void HexahedronSetTopologyModifier::addHexahedraWarning(const unsigned int nHexahedra,
        const sofa::helper::vector< Hexahedron >& hexahedraList,
        const sofa::helper::vector< unsigned int >& hexahedraIndexList,
        const sofa::helper::vector< sofa::helper::vector< unsigned int > > & ancestors,
        const sofa::helper::vector< sofa::helper::vector< double > >& baryCoefs)
{
    // Warning that hexahedra just got created
    HexahedraAdded *e = new HexahedraAdded(nHexahedra, hexahedraList, hexahedraIndexList, ancestors, baryCoefs);
    addTopologyChange(e);
}


void HexahedronSetTopologyModifier::removeHexahedraWarning( sofa::helper::vector<unsigned int> &hexahedra )
{
    /// sort vertices to remove in a descendent order
    std::sort( hexahedra.begin(), hexahedra.end(), std::greater<unsigned int>() );

    // Warning that these edges will be deleted
    HexahedraRemoved *e = new HexahedraRemoved(hexahedra);
    addTopologyChange(e);
}


void HexahedronSetTopologyModifier::removeHexahedraProcess( const sofa::helper::vector<unsigned int> &indices,
        const bool removeIsolatedItems)
{
    if(!m_container->hasHexahedra())
        return;

    bool removeIsolatedVertices = removeIsolatedItems;
    bool removeIsolatedEdges = removeIsolatedItems && m_container->hasEdges();
    bool removeIsolatedQuads = removeIsolatedItems && m_container->hasQuads();

    if(removeIsolatedVertices)
    {
        if(!m_container->hasHexahedronVertexShell())
            m_container->createHexahedronVertexShellArray();
    }

    if(removeIsolatedEdges)
    {
        if(!m_container->hasHexahedronEdgeShell())
            m_container->createHexahedronEdgeShellArray();
    }

    if(removeIsolatedQuads)
    {
        if(!m_container->hasHexahedronQuadShell())
            m_container->createHexahedronQuadShellArray();
    }

    sofa::helper::vector<unsigned int> quadToBeRemoved;
    sofa::helper::vector<unsigned int> edgeToBeRemoved;
    sofa::helper::vector<unsigned int> vertexToBeRemoved;

    unsigned int lastHexahedron = m_container->getNumberOfHexahedra() - 1;
    for(unsigned int i=0; i<indices.size(); ++i, --lastHexahedron)
    {
        Hexahedron &t = m_container->m_hexahedron[ indices[i] ];
        Hexahedron &h = m_container->m_hexahedron[ lastHexahedron ];

        if(m_container->hasHexahedronVertexShell())
        {
            for(unsigned int v=0; v<8; ++v)
            {
                sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronVertexShell[ t[v] ];
                shell.erase(remove(shell.begin(), shell.end(), indices[i]), shell.end());
                if(removeIsolatedVertices && shell.empty())
                    vertexToBeRemoved.push_back(t[v]);
            }
        }

        if(m_container->hasHexahedronEdgeShell())
        {
            for(unsigned int e=0; e<12; ++e)
            {
                sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronEdgeShell[ m_container->m_hexahedronEdge[indices[i]][e]];
                shell.erase(remove(shell.begin(), shell.end(), indices[i]), shell.end());
                if(removeIsolatedEdges && shell.empty())
                    edgeToBeRemoved.push_back(m_container->m_hexahedronEdge[indices[i]][e]);
            }
        }

        if(m_container->hasHexahedronQuadShell())
        {
            for(unsigned int q=0; q<6; ++q)
            {
                sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronQuadShell[ m_container->m_hexahedronQuad[indices[i]][q]];
                shell.erase(remove(shell.begin(), shell.end(), indices[i]), shell.end());
                if(removeIsolatedQuads && shell.empty())
                    quadToBeRemoved.push_back(m_container->m_hexahedronQuad[indices[i]][q]);
            }
        }

        // now updates the shell information of the edge formely at the end of the array
        if( indices[i] < lastHexahedron )
        {
            if(m_container->hasHexahedronVertexShell())
            {
                for(unsigned int v=0; v<8; ++v)
                {
                    sofa::helper::vector< unsigned int > &shell = m_container->m_hexahedronVertexShell[ h[v] ];
                    replace(shell.begin(), shell.end(), lastHexahedron, indices[i]);
                }
            }

            if(m_container->hasHexahedronEdgeShell())
            {
                for(unsigned int e=0; e<12; ++e)
                {
                    sofa::helper::vector< unsigned int > &shell =  m_container->m_hexahedronEdgeShell[ m_container->m_hexahedronEdge[lastHexahedron][e]];
                    replace(shell.begin(), shell.end(), lastHexahedron, indices[i]);
                }
            }

            if(m_container->hasHexahedronQuadShell())
            {
                for(unsigned int q=0; q<6; ++q)
                {
                    sofa::helper::vector< unsigned int > &shell =  m_container->m_hexahedronQuadShell[ m_container->m_hexahedronQuad[lastHexahedron][q]];
                    replace(shell.begin(), shell.end(), lastHexahedron, indices[i]);
                }
            }
        }

        if(m_container->hasHexahedronQuads())
        {
            // removes the hexahedronQuads from the hexahedronQuadArray
            m_container->m_hexahedronQuad[ indices[i] ] = m_container->m_hexahedronQuad[ lastHexahedron ]; // overwriting with last valid value.
            m_container->m_hexahedronQuad.resize( lastHexahedron ); // resizing to erase multiple occurence of the hexa.
        }

        if(m_container->hasHexahedronEdges())
        {
            // removes the hexahedronEdges from the hexahedronEdgeArray
            m_container->m_hexahedronEdge[ indices[i] ] = m_container->m_hexahedronEdge[ lastHexahedron ]; // overwriting with last valid value.
            m_container->m_hexahedronEdge.resize( lastHexahedron ); // resizing to erase multiple occurence of the hexa.
        }

        // removes the hexahedron from the hexahedronArray
        m_container->m_hexahedron[ indices[i] ] = m_container->m_hexahedron[ lastHexahedron ]; // overwriting with last valid value.
        m_container->m_hexahedron.resize( lastHexahedron ); // resizing to erase multiple occurence of the hexa.
    }

    if( (!quadToBeRemoved.empty()) || (!edgeToBeRemoved.empty()))
    {
        if(!quadToBeRemoved.empty())
        {
            /// warn that quads will be deleted
            removeQuadsWarning(quadToBeRemoved);
        }

        if(!edgeToBeRemoved.empty())
        {
            /// warn that edges will be deleted
            removeEdgesWarning(edgeToBeRemoved);
        }

        propagateTopologicalChanges();

        if(!quadToBeRemoved.empty())
        {
            /// actually remove quads without looking for isolated vertices
            removeQuadsProcess(quadToBeRemoved, false, false);
        }

        if(!edgeToBeRemoved.empty())
        {
            /// actually remove edges without looking for isolated vertices
            removeEdgesProcess(edgeToBeRemoved, false);
        }
    }

    if(!vertexToBeRemoved.empty())
    {
        removePointsWarning(vertexToBeRemoved);
        propagateTopologicalChanges();
        removePointsProcess(vertexToBeRemoved);
    }
}

void HexahedronSetTopologyModifier::addPointsProcess(const unsigned int nPoints)
{
    // start by calling the parent's method.
    QuadSetTopologyModifier::addPointsProcess( nPoints );

    if(m_container->hasHexahedronVertexShell())
        m_container->m_hexahedronVertexShell.resize( m_container->getNbPoints() );
}

void HexahedronSetTopologyModifier::addEdgesProcess(const sofa::helper::vector< Edge > &edges)
{
    // start by calling the parent's method.
    QuadSetTopologyModifier::addEdgesProcess( edges );

    if(m_container->hasHexahedronEdgeShell())
        m_container->m_hexahedronEdgeShell.resize( m_container->getNumberOfEdges() );
}

void HexahedronSetTopologyModifier::addQuadsProcess(const sofa::helper::vector< Quad > &quads)
{
    // start by calling the parent's method.
    QuadSetTopologyModifier::addQuadsProcess( quads );

    if(m_container->hasHexahedronQuadShell())
        m_container->m_hexahedronQuadShell.resize( m_container->getNumberOfQuads() );
}

void HexahedronSetTopologyModifier::removePointsProcess( sofa::helper::vector<unsigned int> &indices,
        const bool removeDOF)
{
    if(m_container->hasHexahedra())
    {
        if(!m_container->hasHexahedronVertexShell())
        {
            m_container->createHexahedronVertexShellArray();
        }

        unsigned int lastPoint = m_container->getNbPoints() - 1;
        for(unsigned int i = 0; i < indices.size(); ++i, --lastPoint)
        {
            // updating the edges connected to the point replacing the removed one:
            // for all edges connected to the last point
            for(sofa::helper::vector<unsigned int>::iterator itt=m_container->m_hexahedronVertexShell[lastPoint].begin();
                itt!=m_container->m_hexahedronVertexShell[lastPoint].end(); ++itt)
            {
                unsigned int vertexIndex = m_container->getVertexIndexInHexahedron(m_container->m_hexahedron[*itt], lastPoint);
                m_container->m_hexahedron[*itt][ vertexIndex] = indices[i];
            }

            // updating the edge shell itself (change the old index for the new one)
            m_container->m_hexahedronVertexShell[ indices[i] ] = m_container->m_hexahedronVertexShell[ lastPoint ];
        }

        m_container->m_hexahedronVertexShell.resize( m_container->m_hexahedronVertexShell.size() - indices.size() );
    }

    // Important : the points are actually deleted from the mechanical object's state vectors iff (removeDOF == true)
    // call the parent's method.
    QuadSetTopologyModifier::removePointsProcess(  indices, removeDOF );
}

void HexahedronSetTopologyModifier::removeEdgesProcess( const sofa::helper::vector<unsigned int> &indices,
        const bool removeIsolatedItems)
{
    if(!m_container->hasEdges()) // this method should only be called when edges exist
        return;

    if(m_container->hasHexahedronEdges())
    {
        if(!m_container->hasHexahedronEdgeShell())
            m_container->createHexahedronEdgeShellArray();

        unsigned int lastEdge = m_container->getNumberOfEdges() - 1;
        for(unsigned int i=0; i<indices.size(); ++i, --lastEdge)
        {
            for(sofa::helper::vector<unsigned int>::iterator itt=m_container->m_hexahedronEdgeShell[lastEdge].begin();
                itt!=m_container->m_hexahedronEdgeShell[lastEdge].end(); ++itt)
            {
                unsigned int edgeIndex = m_container->getEdgeIndexInHexahedron(m_container->m_hexahedronEdge[*itt], lastEdge);
                m_container->m_hexahedronEdge[*itt][edgeIndex] = indices[i];
            }

            // updating the edge shell itself (change the old index for the new one)
            m_container->m_hexahedronEdgeShell[ indices[i] ] = m_container->m_hexahedronEdgeShell[ lastEdge ];
        }

        m_container->m_hexahedronEdgeShell.resize( m_container->m_hexahedronEdgeShell.size() - indices.size() );
    }

    // call the parent's method.
    QuadSetTopologyModifier::removeEdgesProcess(  indices, removeIsolatedItems );
}

void HexahedronSetTopologyModifier::removeQuadsProcess( const sofa::helper::vector<unsigned int> &indices,
        const bool removeIsolatedEdges,
        const bool removeIsolatedPoints)
{
    if(!m_container->hasQuads()) // this method should only be called when quads exist
        return;

    if(m_container->hasHexahedronQuads())
    {
        if(!m_container->hasHexahedronQuadShell())
            m_container->createHexahedronQuadShellArray();

        unsigned int lastQuad = m_container->getNumberOfQuads() - 1;
        for(unsigned int i=0; i<indices.size(); ++i, --lastQuad)
        {
            for(sofa::helper::vector<unsigned int>::iterator itt=m_container->m_hexahedronQuadShell[lastQuad].begin();
                itt!=m_container->m_hexahedronQuadShell[lastQuad].end(); ++itt)
            {
                unsigned int quadIndex=m_container->getQuadIndexInHexahedron(m_container->m_hexahedronQuad[*itt],lastQuad);
                m_container->m_hexahedronQuad[*itt][quadIndex]=indices[i];
            }

            // updating the quad shell itself (change the old index for the new one)
            m_container->m_hexahedronQuadShell[ indices[i] ] = m_container->m_hexahedronQuadShell[ lastQuad ];
        }
        m_container->m_hexahedronQuadShell.resize( m_container->m_hexahedronQuadShell.size() - indices.size() );
    }

    // call the parent's method.
    QuadSetTopologyModifier::removeQuadsProcess( indices, removeIsolatedEdges, removeIsolatedPoints );
}

void HexahedronSetTopologyModifier::renumberPointsProcess( const sofa::helper::vector<unsigned int> &index, const sofa::helper::vector<unsigned int> &inv_index, const bool renumberDOF)
{
    if(m_container->hasHexahedra())
    {
        if(m_container->hasHexahedronVertexShell())
        {
            sofa::helper::vector< sofa::helper::vector< unsigned int > > hexahedronVertexShell_cp = m_container->m_hexahedronVertexShell;
            for(unsigned int i=0; i<index.size(); ++i)
            {
                m_container->m_hexahedronVertexShell[i] = hexahedronVertexShell_cp[ index[i] ];
            }
        }

        for(unsigned int i=0; i<m_container->m_hexahedron.size(); ++i)
        {
            m_container->m_hexahedron[i][0]  = inv_index[ m_container->m_hexahedron[i][0]  ];
            m_container->m_hexahedron[i][1]  = inv_index[ m_container->m_hexahedron[i][1]  ];
            m_container->m_hexahedron[i][2]  = inv_index[ m_container->m_hexahedron[i][2]  ];
            m_container->m_hexahedron[i][3]  = inv_index[ m_container->m_hexahedron[i][3]  ];
            m_container->m_hexahedron[i][4]  = inv_index[ m_container->m_hexahedron[i][4]  ];
            m_container->m_hexahedron[i][5]  = inv_index[ m_container->m_hexahedron[i][5]  ];
            m_container->m_hexahedron[i][6]  = inv_index[ m_container->m_hexahedron[i][6]  ];
            m_container->m_hexahedron[i][7]  = inv_index[ m_container->m_hexahedron[i][7]  ];
        }
    }

    // call the parent's method.
    QuadSetTopologyModifier::renumberPointsProcess( index, inv_index, renumberDOF );
}

void HexahedronSetTopologyModifier::removeHexahedra(sofa::helper::vector< unsigned int >& hexahedra)
{
    // add the topological changes in the queue
    removeHexahedraWarning(hexahedra);
    // inform other objects that the hexa are going to be removed
    propagateTopologicalChanges();
    // now destroy the old hexahedra.
    removeHexahedraProcess(  hexahedra ,true);

    m_container->checkTopology();
}

void HexahedronSetTopologyModifier::removeItems(sofa::helper::vector< unsigned int >& items)
{
    removeHexahedra(items);
}

void HexahedronSetTopologyModifier::renumberPoints(const sofa::helper::vector<unsigned int> &index,
        const sofa::helper::vector<unsigned int> &inv_index)
{
    /// add the topological changes in the queue
    renumberPointsWarning(index, inv_index);
    // inform other objects that the triangles are going to be removed
    propagateTopologicalChanges();
    // now renumber the points
    renumberPointsProcess(index, inv_index);

    m_container->checkTopology();
}

} // namespace topology

} // namespace component

} // namespace sofa

