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
#include <iostream>
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/fixed_array.h>
#include <sofa/component/MeshLoader.h>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/gl/template.h>
#include <set>
#include <string.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;
using core::componentmodel::topology::BaseMeshTopology;

SOFA_DECL_CLASS(MeshTopology)

int MeshTopologyClass = core::RegisterObject("Generic mesh topology")
        .addAlias("Mesh")
        .add< MeshTopology >()
        ;

MeshTopology::MeshTopology()
    : nbPoints(0)
    , seqEdges(initData(&seqEdges,"edges","List of edge indices")), validEdges(false)
    , seqTriangles(initData(&seqTriangles,"triangles","List of triangle indices")), validTriangles(false)
    , seqQuads(initData(&seqQuads,"quads","List of quad indices")), validQuads(false)
    , seqTetras(initData(&seqTetras,"tetras","List of tetrahedron indices")), validTetras(false)
    , seqHexas(initData(&seqHexas,"hexas","List of hexahedron indices")), validHexas(false)
    , revision(0)
    , _draw(initData(&_draw, false, "drawHexas","if true, draw the topology hexahedra"))
{
}

void MeshTopology::parse(core::objectmodel::BaseObjectDescription* arg)
{
    if (arg->getAttribute("lines")) // old name for edges
    {
        std::string s = arg->getAttribute("lines");
        seqEdges.read(s);
    }
    BaseMeshTopology::parse(arg);
}

void MeshTopology::init()
{
    sofa::component::MeshLoader* loader;
    this->getContext()->get(loader);

    if(loader)
    {
        loadFromMeshLoader(loader);
    }

    // compute the number of points, if the topology is charged from the scene or if it was loaded from a MeshLoader without any points data.
    if (nbPoints==0)
    {
        unsigned int n = 0;
        for (unsigned int i=0; i<seqEdges.getValue().size(); i++)
        {
            for (unsigned int j=0; j<seqEdges.getValue()[i].size(); j++)
            {
                if (n <= seqEdges.getValue()[i][j])
                    n = 1 + seqEdges.getValue()[i][j];
            }
        }
        for (unsigned int i=0; i<seqTriangles.getValue().size(); i++)
        {
            for (unsigned int j=0; j<seqTriangles.getValue()[i].size(); j++)
            {
                if (n <= seqTriangles.getValue()[i][j])
                    n = 1 + seqTriangles.getValue()[i][j];
            }
        }
        for (unsigned int i=0; i<seqQuads.getValue().size(); i++)
        {
            for (unsigned int j=0; j<seqQuads.getValue()[i].size(); j++)
            {
                if (n <= seqQuads.getValue()[i][j])
                    n = 1 + seqQuads.getValue()[i][j];
            }
        }
        for (unsigned int i=0; i<seqTetras.getValue().size(); i++)
        {
            for (unsigned int j=0; j<seqTetras.getValue()[i].size(); j++)
            {
                if (n <= seqTetras.getValue()[i][j])
                    n = 1 + seqTetras.getValue()[i][j];
            }
        }
        for (unsigned int i=0; i<seqHexas.getValue().size(); i++)
        {
            for (unsigned int j=0; j<seqHexas.getValue()[i].size(); j++)
            {
                if (n <= seqHexas.getValue()[i][j])
                    n = 1 + seqHexas.getValue()[i][j];
            }
        }
        nbPoints = n;
    }
}

void MeshTopology::loadFromMeshLoader(sofa::component::MeshLoader* loader)
{
    nbPoints = loader->getNbPoints();
    seqPoints = loader->getPoints();
    seqEdges = loader->getEdges();
    seqTriangles = loader->getTriangles();
    seqQuads = loader->getQuads();
    seqTetras = loader->getTetras();
    seqHexas = loader->getHexas();
}

void MeshTopology::clear()
{
    nbPoints = 0;
    seqEdges.beginEdit()->clear(); seqEdges.endEdit();
    seqTriangles.beginEdit()->clear(); seqTriangles.endEdit();
    seqQuads.beginEdit()->clear(); seqQuads.endEdit();
    seqTetras.beginEdit()->clear(); seqTetras.endEdit();
    seqHexas.beginEdit()->clear(); seqHexas.endEdit();
    invalidate();
}


void MeshTopology::addPoint(double px, double py, double pz)
{
    seqPoints.push_back(helper::make_array((SReal)px, (SReal)py, (SReal)pz));
    if (seqPoints.size() > (unsigned)nbPoints)
        nbPoints = seqPoints.size();
}

void MeshTopology::addEdge( int a, int b )
{
    seqEdges.beginEdit()->push_back(Edge(a,b));
    seqEdges.endEdit();
    if (a >= (int)nbPoints) nbPoints = a+1;
    if (b >= (int)nbPoints) nbPoints = b+1;
}

void MeshTopology::addTriangle( int a, int b, int c )
{
    seqTriangles.beginEdit()->push_back( Triangle(a,b,c) );
    seqTriangles.endEdit();
    if (a >= (int)nbPoints) nbPoints = a+1;
    if (b >= (int)nbPoints) nbPoints = b+1;
    if (c >= (int)nbPoints) nbPoints = c+1;
}

void MeshTopology::addQuad(int a, int b, int c, int d)
{
    seqQuads.beginEdit()->push_back(Quad(a,b,c,d));
    seqQuads.endEdit();
    if (a >= (int)nbPoints) nbPoints = a+1;
    if (b >= (int)nbPoints) nbPoints = b+1;
    if (c >= (int)nbPoints) nbPoints = c+1;
    if (d >= (int)nbPoints) nbPoints = d+1;
}

void MeshTopology::addTetra( int a, int b, int c, int d )
{
    seqTetras.beginEdit()->push_back( Tetra(a,b,c,d) );
    seqTetras.endEdit();
    if (a >= (int)nbPoints) nbPoints = a+1;
    if (b >= (int)nbPoints) nbPoints = b+1;
    if (c >= (int)nbPoints) nbPoints = c+1;
    if (d >= (int)nbPoints) nbPoints = d+1;
}

void MeshTopology::addHexa(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
#ifdef SOFA_NEW_HEXA
    seqHexas.beginEdit()->push_back(Hexa(p1,p2,p3,p4,p5,p6,p7,p8));
#else
    seqHexas.beginEdit()->push_back(Hexa(p1,p2,p4,p3,p5,p6,p8,p7));
#endif
    seqHexas.endEdit();
    if (p1 >= (int)nbPoints) nbPoints = p1+1;
    if (p2 >= (int)nbPoints) nbPoints = p2+1;
    if (p3 >= (int)nbPoints) nbPoints = p3+1;
    if (p4 >= (int)nbPoints) nbPoints = p4+1;
    if (p5 >= (int)nbPoints) nbPoints = p5+1;
    if (p6 >= (int)nbPoints) nbPoints = p6+1;
    if (p7 >= (int)nbPoints) nbPoints = p7+1;
    if (p8 >= (int)nbPoints) nbPoints = p8+1;
}

const MeshTopology::SeqEdges& MeshTopology::getEdges()
{
    if (!validEdges)
    {
        updateEdges();
        validEdges = true;
    }
    return seqEdges.getValue();
}

const MeshTopology::SeqTriangles& MeshTopology::getTriangles()
{
    if (!validTriangles)
    {
        updateTriangles();
        validTriangles = true;
    }
    return seqTriangles.getValue();
}

const MeshTopology::SeqQuads& MeshTopology::getQuads()
{
    if (!validQuads)
    {
        updateQuads();
        validQuads = true;
    }
    return seqQuads.getValue();
}

const MeshTopology::SeqTetras& MeshTopology::getTetras()
{
    if (!validTetras)
    {
        updateTetras();
        validTetras = true;
    }
    return seqTetras.getValue();
}

const MeshTopology::SeqHexas& MeshTopology::getHexas()
{
    if (!validHexas)
    {
        updateHexas();
        validHexas = true;
    }
    return seqHexas.getValue();
}

int MeshTopology::getNbPoints() const
{
    return nbPoints;
}

void MeshTopology::setNbPoints(int n)
{
    nbPoints = n;
}

int MeshTopology::getNbEdges()
{
    return getEdges().size();
}

int MeshTopology::getNbTriangles()
{
    return getTriangles().size();
}

int MeshTopology::getNbQuads()
{
    return getQuads().size();
}

int MeshTopology::getNbTetras()
{
    return getTetras().size();
}

int MeshTopology::getNbHexas()
{
    return getHexas().size();
}

MeshTopology::Edge MeshTopology::getEdge(index_type i)
{
    return getEdges()[i];
}

MeshTopology::Triangle MeshTopology::getTriangle(index_type i)
{
    return getTriangles()[i];
}

MeshTopology::Quad MeshTopology::getQuad(index_type i)
{
    return getQuads()[i];
}

MeshTopology::Tetra MeshTopology::getTetra(index_type i)
{
    return getTetras()[i];
}

MeshTopology::Hexa MeshTopology::getHexa(index_type i)
{
    return getHexas()[i];
}

void MeshTopology::createEdgeVertexShellArray ()
{
    const SeqEdges& edges = getEdges();
    m_edgeVertexShell.clear();
    m_edgeVertexShell.resize( nbPoints );
    for (unsigned int i = 0; i < edges.size(); ++i)
    {
        // adding edge i in the edge shell of both points
        m_edgeVertexShell[ edges[i][0] ].push_back( i );
        m_edgeVertexShell[ edges[i][1] ].push_back( i );
    }
}

void MeshTopology::createEdgeTriangleShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqTriangles& triangles = getTriangles(); // do not use seqTriangles directly as it might not be up-to-date
    m_edgeTriangleShell.clear();
    m_edgeTriangleShell.resize(triangles.size());
    for (unsigned int i = 0; i < triangles.size(); ++i)
    {
        const Triangle &t=triangles[i];
        // adding edge i in the edge shell of both points
        for (unsigned int j=0; j<3; ++j)
        {
            int edgeIndex=getEdgeIndex(t[(j+1)%3],t[(j+2)%3]);
            assert(edgeIndex!= -1);
            m_edgeTriangleShell[i][j]=edgeIndex;
        }
    }
}

void MeshTopology::createEdgeQuadShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqQuads& quads = getQuads(); // do not use seqQuads directly as it might not be up-to-date
    m_edgeQuadShell.clear();
    m_edgeQuadShell.resize(quads.size());
    for (unsigned int i = 0; i < quads.size(); ++i)
    {
        const Quad &t=quads[i];
        // adding edge i in the edge shell of both points
        for (unsigned int j=0; j<4; ++j)
        {
            int edgeIndex=getEdgeIndex(t[(j+1)%4],t[(j+2)%4]);
            assert(edgeIndex!= -1);
            m_edgeQuadShell[i][j]=edgeIndex;
        }
    }
}

void MeshTopology::createEdgeTetraShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqQuads& tetras = getTetras(); // do not use seqTetras directly as it might not be up-to-date
    m_edgeTetraShell.clear();
    m_edgeTetraShell.resize(tetras.size());
    const unsigned int tetrahedronEdgeArray[6][2]= {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};

    for (unsigned int i = 0; i < tetras.size(); ++i)
    {
        const Tetra &t=tetras[i];
        // adding edge i in the edge shell of both points
        for (unsigned int j=0; j<6; ++j)
        {
            int edgeIndex=getEdgeIndex(t[tetrahedronEdgeArray[j][0]],
                    t[tetrahedronEdgeArray[j][1]]);
            assert(edgeIndex!= -1);
            m_edgeTetraShell[i][j]=edgeIndex;
        }
    }
}

void MeshTopology::createEdgeHexaShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqHexas& hexas = getHexas(); // do not use seqHexas directly as it might not be up-to-date
    m_edgeHexaShell.clear();
    m_edgeHexaShell.resize(hexas.size());
    const unsigned int edgeHexahedronDescriptionArray[12][2]= {{0,1},{0,3},{0,4},{1,2},{1,5},{2,3},{2,6},{3,7},{4,5},{4,7},{5,6},{6,7}};

    for (unsigned int i = 0; i < hexas.size(); ++i)
    {
        const Hexa &h=hexas[i];
        // adding edge i in the edge shell of both points
        for (unsigned int j=0; j<12; ++j)
        {
            int edgeIndex=getEdgeIndex(h[edgeHexahedronDescriptionArray[j][0]],
                    h[edgeHexahedronDescriptionArray[j][1]]);
            assert(edgeIndex!= -1);
            m_edgeHexaShell[i][j]=edgeIndex;
        }
    }
}

void MeshTopology::createTriangleVertexShellArray ()
{
    const SeqTriangles& triangles = getTriangles(); // do not use seqTriangles directly as it might not be up-to-date
    m_triangleVertexShell.clear();
    m_triangleVertexShell.resize( nbPoints );

    for (unsigned int i = 0; i < triangles.size(); ++i)
    {
        // adding triangle i in the triangle shell of all points
        for (unsigned int j=0; j<3; ++j)
            m_triangleVertexShell[ triangles[i][j]  ].push_back( i );
    }
}

void MeshTopology::createTriangleEdgeShellArray ()
{
    const SeqTriangles& triangles = getTriangles(); // do not use seqTriangles directly as it might not be up-to-date
    if (m_edgeTriangleShell.empty())
        createEdgeTriangleShellArray();
    m_triangleEdgeShell.clear();
    m_triangleEdgeShell.resize( getNbEdges());
    const vector< TriangleEdges > &tea=m_edgeTriangleShell;
    unsigned int j;

    for (unsigned int i = 0; i < triangles.size(); ++i)
    {
        const Triangle &t=triangles[i];
        // adding triangle i in the triangle shell of all edges
        for (j=0; j<3; ++j)
        {
            if (seqEdges.getValue()[tea[i][j]][0] == t[(j+1)%3])
                m_triangleEdgeShell[ tea[i][j] ].insert(m_triangleEdgeShell[ tea[i][j] ].begin(), i); // triangle is on the left of the edge
            else
                m_triangleEdgeShell[ tea[i][j] ].push_back( i ); // triangle is on the right of the edge
        }
    }
}

void MeshTopology::createTriangleTetraShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqQuads& tetras = getTetras(); // do not use seqTetras directly as it might not be up-to-date
    m_triangleTetraShell.clear();
    m_triangleTetraShell.resize(tetras.size());

    for (unsigned int i = 0; i < tetras.size(); ++i)
    {
        const Tetra &t=tetras[i];
        // adding triangles in the triangle list of the ith tetrahedron  i
        for (unsigned int j=0; j<4; ++j)
        {
            int triangleIndex=getTriangleIndex(t[(j+1)%4],t[(j+2)%4],t[(j+3)%4]);
            assert(triangleIndex!= -1);
            m_triangleTetraShell[i][j]=triangleIndex;
        }
    }
}


void MeshTopology::createQuadVertexShellArray ()
{
    const SeqQuads& quads = getQuads(); // do not use seqQuads directly as it might not be up-to-date
    m_quadVertexShell.clear();
    m_quadVertexShell.resize( nbPoints );

    for (unsigned int i = 0; i < quads.size(); ++i)
    {
        // adding quad i in the quad shell of all points
        for (unsigned j=0; j<4; ++j)
            m_quadVertexShell[ quads[i][j]  ].push_back( i );
    }
}

void MeshTopology::createQuadEdgeShellArray ()
{
    const SeqQuads& quads = getQuads(); // do not use seqQuads directly as it might not be up-to-date
    if (m_edgeQuadShell.empty())
        createEdgeQuadShellArray();
    m_quadEdgeShell.clear();
    m_quadEdgeShell.resize( getNbEdges() );
    unsigned int j;
    for (unsigned int i = 0; i < quads.size(); ++i)
    {
        for (j=0; j<4; ++j)
            m_quadEdgeShell[ m_edgeQuadShell[i][j] ].push_back( i );
    }
}

void MeshTopology::createQuadHexaShellArray ()
{
    //const SeqEdges& edges = getEdges(); // do not use seqEdges directly as it might not be up-to-date
    const SeqHexas& hexas = getHexas(); // do not use seqHexas directly as it might not be up-to-date
    m_quadHexaShell.clear();
    m_quadHexaShell.resize(hexas.size());

    for (unsigned int i = 0; i < hexas.size(); ++i)
    {
        const Hexa &h=hexas[i];
        int quadIndex;
        // adding the 6 quads in the quad list of the ith hexahedron  i
        // Quad 0 :
        quadIndex=getQuadIndex(h[0],h[3],h[2],h[1]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][0]=quadIndex;
        // Quad 1 :
        quadIndex=getQuadIndex(h[4],h[5],h[6],h[7]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][1]=quadIndex;
        // Quad 2 :
        quadIndex=getQuadIndex(h[0],h[1],h[5],h[4]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][2]=quadIndex;
        // Quad 3 :
        quadIndex=getQuadIndex(h[1],h[2],h[6],h[5]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][3]=quadIndex;
        // Quad 4 :
        quadIndex=getQuadIndex(h[2],h[3],h[7],h[6]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][4]=quadIndex;
        // Quad 5 :
        quadIndex=getQuadIndex(h[3],h[0],h[4],h[7]);
        assert(quadIndex!= -1);
        m_quadHexaShell[i][5]=quadIndex;
    }
}

void MeshTopology::createTetraVertexShellArray ()
{
    m_tetraVertexShell.resize( nbPoints );
    unsigned int j;

    for (unsigned int i = 0; i < seqTetras.getValue().size(); ++i)
    {
        for (j=0; j<4; ++j)
            m_tetraVertexShell[ seqTetras.getValue()[i][j]  ].push_back( i );
    }
}

void MeshTopology::createTetraEdgeShellArray ()
{
    if (!m_edgeTetraShell.size())
        createEdgeTetraShellArray();
    m_tetraEdgeShell.resize( getNbEdges() );
    const vector< TetraEdges > &tea = m_edgeTetraShell;
    unsigned int j;

    for (unsigned int i = 0; i < seqTetras.getValue().size(); ++i)
    {
        for (j=0; j<6; ++j)
            m_tetraEdgeShell[ tea[i][j] ].push_back( i );
    }
}

void MeshTopology::createTetraTriangleShellArray ()
{
    if (!m_triangleTetraShell.size())
        createTriangleTetraShellArray();
    m_tetraTriangleShell.resize( getNbTriangles());
    unsigned int j;
    const vector< TetraTriangles > &tta=m_triangleTetraShell;

    for (unsigned int i = 0; i < seqTetras.getValue().size(); ++i)
    {
        for (j=0; j<4; ++j)
            m_tetraTriangleShell[ tta[i][j] ].push_back( i );
    }
}

void MeshTopology::createHexaVertexShellArray ()
{
    m_hexaVertexShell.resize( nbPoints );
    unsigned int j;

    for (unsigned int i = 0; i < seqHexas.getValue().size(); ++i)
    {
        for (j=0; j<8; ++j)
            m_hexaVertexShell[ seqHexas.getValue()[i][j]  ].push_back( i );
    }
}

void MeshTopology::createHexaEdgeShellArray ()
{
    if (!m_edgeHexaShell.size())
        createEdgeHexaShellArray();
    m_hexaEdgeShell.resize(getNbEdges());
    unsigned int j;
    const vector< HexaEdges > &hea=m_edgeHexaShell;

    for (unsigned int i = 0; i < seqHexas.getValue().size(); ++i)
    {
        for (j=0; j<12; ++j)
            m_hexaEdgeShell[ hea[i][j] ].push_back( i );
    }
}

void MeshTopology::createHexaQuadShellArray ()
{
    if (!m_quadHexaShell.size())
        createQuadHexaShellArray();
    m_hexaQuadShell.resize( getNbQuads());
    unsigned int j;
    const vector< HexaQuads > &qha=m_quadHexaShell;

    for (unsigned int i = 0; i < seqHexas.getValue().size(); ++i)
    {
        // adding quad i in the edge shell of both points
        for (j=0; j<6; ++j)
            m_hexaQuadShell[ qha[i][j] ].push_back( i );
    }
}

const MeshTopology::VertexEdges& MeshTopology::getEdgeVertexShell(PointID i)
{
    if (!m_edgeVertexShell.size() || i > m_edgeVertexShell.size()-1)
        createEdgeVertexShellArray();
    return m_edgeVertexShell[i];
}

const MeshTopology::TriangleEdges& MeshTopology::getEdgeTriangleShell(TriangleID i)
{
    if (m_edgeTriangleShell.empty() || i > m_edgeTriangleShell.size()-1)
        createEdgeTriangleShellArray();
    return m_edgeTriangleShell[i];
}

const MeshTopology::QuadEdges& MeshTopology::getEdgeQuadShell(QuadID i)
{
    if (m_edgeQuadShell.empty() || i > m_edgeQuadShell.size()-1)
        createEdgeQuadShellArray();
    return m_edgeQuadShell[i];
}

const MeshTopology::TetraEdges& MeshTopology::getEdgeTetraShell(TetraID i)
{
    if (!m_edgeTetraShell.empty() || i > m_edgeTetraShell.size()-1)
        createEdgeTetraShellArray();
    return m_edgeTetraShell[i];
}

const MeshTopology::HexaEdges& MeshTopology::getEdgeHexaShell(HexaID i)
{
    if (!m_edgeHexaShell.size() || i > m_edgeHexaShell.size()-1)
        createEdgeHexaShellArray();
    return m_edgeHexaShell[i];
}

const MeshTopology::VertexTriangles& MeshTopology::getTriangleVertexShell(PointID i)
{
    if (!m_triangleVertexShell.size() || i > m_triangleVertexShell.size()-1)
        createTriangleVertexShellArray();
    return m_triangleVertexShell[i];
}

const MeshTopology::EdgeTriangles& MeshTopology::getTriangleEdgeShell(EdgeID i)
{
    if (m_triangleEdgeShell.empty() || i > m_triangleEdgeShell.size()-1)
        createTriangleEdgeShellArray();
    return m_triangleEdgeShell[i];
}

const MeshTopology::TetraTriangles& MeshTopology::getTriangleTetraShell(TetraID i)
{
    if (!m_triangleTetraShell.size() || i > m_triangleTetraShell.size()-1)
        createTriangleTetraShellArray();
    return m_triangleTetraShell[i];
}

const MeshTopology::VertexQuads& MeshTopology::getQuadVertexShell(PointID i)
{
    if (m_quadVertexShell.empty() || i > m_quadVertexShell.size()-1)
        createQuadVertexShellArray();
    return m_quadVertexShell[i];
}

const vector< MeshTopology::QuadID >& MeshTopology::getQuadEdgeShell(EdgeID i)
{
    if (!m_quadEdgeShell.size() || i > m_quadEdgeShell.size()-1)
        createQuadEdgeShellArray();
    return m_quadEdgeShell[i];
}

const MeshTopology::HexaQuads& MeshTopology::getQuadHexaShell(HexaID i)
{
    if (!m_quadHexaShell.size() || i > m_quadHexaShell.size()-1)
        createQuadHexaShellArray();
    return m_quadHexaShell[i];
}

const MeshTopology::VertexTetras& MeshTopology::getTetraVertexShell(PointID i)
{
    if (!m_tetraVertexShell.size() || i > m_tetraVertexShell.size()-1)
        createTetraVertexShellArray();
    return m_tetraVertexShell[i];
}

const MeshTopology::EdgeTetras& MeshTopology::getTetraEdgeShell(EdgeID i)
{
    if (!m_tetraEdgeShell.size() || i > m_tetraEdgeShell.size()-1)
        createTetraEdgeShellArray();
    return m_tetraEdgeShell[i];
}

const MeshTopology::TriangleTetras& MeshTopology::getTetraTriangleShell(TriangleID i)
{
    if (!m_tetraTriangleShell.size() || i > m_tetraTriangleShell.size()-1)
        createTetraTriangleShellArray();
    return m_tetraTriangleShell[i];
}

const MeshTopology::VertexHexas& MeshTopology::getHexaVertexShell(PointID i)
{
    if (!m_hexaVertexShell.size() || i > m_hexaVertexShell.size()-1)
        createHexaVertexShellArray();
    return m_hexaVertexShell[i];
}

const MeshTopology::EdgeHexas& MeshTopology::getHexaEdgeShell(EdgeID i)
{
    if (!m_hexaEdgeShell.size() || i > m_hexaEdgeShell.size()-1)
        createHexaEdgeShellArray();
    return m_hexaEdgeShell[i];
}

const MeshTopology::QuadHexas& MeshTopology::getHexaQuadShell(QuadID i)
{
    if (!m_hexaQuadShell.size() || i > m_hexaQuadShell.size()-1)
        createHexaQuadShellArray();
    return m_hexaQuadShell[i];
}

const vector< MeshTopology::VertexTriangles >& MeshTopology::getTriangleVertexShellArray()
{
    if (!m_triangleVertexShell.size())
        createTriangleVertexShellArray();
    return m_triangleVertexShell;
}

const vector< MeshTopology::VertexQuads >& MeshTopology::getQuadVertexShellArray()
{
    if (!m_quadVertexShell.size())
        createQuadVertexShellArray();
    return m_quadVertexShell;
}

int MeshTopology::getEdgeIndex(PointID v1, PointID v2)
{
    const VertexEdges &es1 = getEdgeVertexShell(v1) ;
    const SeqEdges &ea = getEdges();
    unsigned int i=0;
    int result= -1;
    while ((i<es1.size()) && (result== -1))
    {
        const MeshTopology::Edge &e=ea[es1[i]];
        if ((e[0]==v2)|| (e[1]==v2))
            result=(int) es1[i];

        i++;
    }
    return result;
}

int MeshTopology::getTriangleIndex(PointID v1, PointID v2, PointID v3)
{
    //const vector< VertexTriangles > &tvs=getTriangleVertexShellArray();

    const vector<TriangleID> &set1=getTriangleVertexShell(v1);
    const vector<TriangleID> &set2=getTriangleVertexShell(v2);
    const vector<TriangleID> &set3=getTriangleVertexShell(v3);

    // The destination vector must be large enough to contain the result.
    vector<TriangleID> out1(set1.size()+set2.size());
    vector<TriangleID>::iterator result1;
    result1 = std::set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),out1.begin());
    out1.erase(result1,out1.end());

    vector<TriangleID> out2(set3.size()+out1.size());
    vector<TriangleID>::iterator result2;
    result2 = std::set_intersection(set3.begin(),set3.end(),out1.begin(),out1.end(),out2.begin());
    out2.erase(result2,out2.end());

    assert(out2.size()==0 || out2.size()==1);

    if (out2.size()==1)
        return (int) (out2[0]);
    else
        return -1;
}

int MeshTopology::getQuadIndex(PointID v1, PointID v2, PointID v3,  PointID v4)
{
    //const vector< VertexQuads > &qvs=getQuadVertexShellArray();

    const vector<QuadID> &set1=getQuadVertexShell(v1);
    const vector<QuadID> &set2=getQuadVertexShell(v2);
    const vector<QuadID> &set3=getQuadVertexShell(v3);
    const vector<QuadID> &set4=getQuadVertexShell(v4);

    // The destination vector must be large enough to contain the result.
    vector<QuadID> out1(set1.size()+set2.size());
    vector<QuadID>::iterator result1;
    result1 = std::set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),out1.begin());
    out1.erase(result1,out1.end());

    vector<QuadID> out2(set3.size()+out1.size());
    vector<QuadID>::iterator result2;
    result2 = std::set_intersection(set3.begin(),set3.end(),out1.begin(),out1.end(),out2.begin());
    out2.erase(result2,out2.end());

    vector<QuadID> out3(set4.size()+out2.size());
    vector<QuadID>::iterator result3;
    result3 = std::set_intersection(set4.begin(),set4.end(),out2.begin(),out2.end(),out3.begin());
    out3.erase(result3,out3.end());

    assert(out3.size()==0 || out3.size()==1);

    if (out3.size()==1)
        return (int) (out3[0]);
    else
        return -1;
}

int MeshTopology::getTetrahedronIndex(PointID v1, PointID v2, PointID v3,  PointID v4)
{
    const vector<TetraID> &set1=getTetraVertexShell(v1);
    const vector<TetraID> &set2=getTetraVertexShell(v2);
    const vector<TetraID> &set3=getTetraVertexShell(v3);
    const vector<TetraID> &set4=getTetraVertexShell(v4);

    // The destination vector must be large enough to contain the result.
    vector<TetraID> out1(set1.size()+set2.size());
    vector<TetraID>::iterator result1;
    result1 = std::set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),out1.begin());
    out1.erase(result1,out1.end());

    vector<TetraID> out2(set3.size()+out1.size());
    vector<TetraID>::iterator result2;
    result2 = std::set_intersection(set3.begin(),set3.end(),out1.begin(),out1.end(),out2.begin());
    out2.erase(result2,out2.end());

    vector<TetraID> out3(set4.size()+out2.size());
    vector<TetraID>::iterator result3;
    result3 = std::set_intersection(set4.begin(),set4.end(),out2.begin(),out2.end(),out3.begin());
    out3.erase(result3,out3.end());

    assert(out3.size()==0 || out3.size()==1);

    if (out3.size()==1)
        return (int) (out3[0]);
    else
        return -1;
}

int MeshTopology::getHexahedronIndex(PointID v1, PointID v2, PointID v3, PointID v4, PointID v5, PointID v6, PointID v7, PointID v8)
{
    const vector<HexaID> &set1=getTetraVertexShell(v1);
    const vector<HexaID> &set2=getTetraVertexShell(v2);
    const vector<HexaID> &set3=getTetraVertexShell(v3);
    const vector<HexaID> &set4=getTetraVertexShell(v4);
    const vector<HexaID> &set5=getTetraVertexShell(v5);
    const vector<HexaID> &set6=getTetraVertexShell(v6);
    const vector<HexaID> &set7=getTetraVertexShell(v7);
    const vector<HexaID> &set8=getTetraVertexShell(v8);

    // The destination vector must be large enough to contain the result.
    vector<HexaID> out1(set1.size()+set2.size());
    vector<HexaID>::iterator result1;
    result1 = std::set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),out1.begin());
    out1.erase(result1,out1.end());

    vector<HexaID> out2(set3.size()+out1.size());
    vector<HexaID>::iterator result2;
    result2 = std::set_intersection(set3.begin(),set3.end(),out1.begin(),out1.end(),out2.begin());
    out2.erase(result2,out2.end());

    vector<HexaID> out3(set4.size()+out2.size());
    vector<HexaID>::iterator result3;
    result3 = std::set_intersection(set4.begin(),set4.end(),out2.begin(),out2.end(),out3.begin());
    out3.erase(result3,out3.end());

    vector<HexaID> out4(set5.size()+set3.size());
    vector<HexaID>::iterator result4;
    result4 = std::set_intersection(set5.begin(),set5.end(),set3.begin(),set3.end(),out4.begin());
    out4.erase(result4,out4.end());

    vector<HexaID> out5(set6.size()+out4.size());
    vector<HexaID>::iterator result5;
    result5 = std::set_intersection(set6.begin(),set6.end(),out4.begin(),out4.end(),out5.begin());
    out5.erase(result5,out5.end());

    vector<HexaID> out6(set7.size()+out5.size());
    vector<HexaID>::iterator result6;
    result6 = std::set_intersection(set7.begin(),set7.end(),out5.begin(),out5.end(),out6.begin());
    out6.erase(result6,out6.end());

    vector<HexaID> out7(set8.size()+out6.size());
    vector<HexaID>::iterator result7;
    result7 = std::set_intersection(set8.begin(),set8.end(),out6.begin(),out6.end(),out7.begin());
    out7.erase(result6,out7.end());

    assert(out7.size()==0 || out7.size()==1);

    if (out7.size()==1)
        return (int) (out7[0]);
    else
        return -1;
}

int MeshTopology::getVertexIndexInTriangle(const Triangle &t, PointID vertexIndex) const
{
    if (t[0]==vertexIndex)
        return 0;
    else if (t[1]==vertexIndex)
        return 1;
    else if (t[2]==vertexIndex)
        return 2;
    else
        return -1;
}

int MeshTopology::getEdgeIndexInTriangle(const TriangleEdges &t, EdgeID edgeIndex) const
{
    if (t[0]==edgeIndex)
        return 0;
    else if (t[1]==edgeIndex)
        return 1;
    else if (t[2]==edgeIndex)
        return 2;
    else
        return -1;
}

int MeshTopology::getVertexIndexInQuad(Quad &t, PointID vertexIndex) const
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

int MeshTopology::getEdgeIndexInQuad(QuadEdges &t, EdgeID edgeIndex) const
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

int MeshTopology::getVertexIndexInTetrahedron(const Tetra &t, PointID vertexIndex) const
{
    if (t[0]==vertexIndex)
        return 0;
    else if (t[1]==vertexIndex)
        return 1;
    else if (t[2]==vertexIndex)
        return 2;
    else if (t[3]==vertexIndex)
        return 3;
    else
        return -1;
}

int MeshTopology::getEdgeIndexInTetrahedron(const TetraEdges &t, EdgeID edgeIndex) const
{
    if (t[0]==edgeIndex)
        return 0;
    else if (t[1]==edgeIndex)
        return 1;
    else if (t[2]==edgeIndex)
        return 2;
    else if (t[3]==edgeIndex)
        return 3;
    else if (t[4]==edgeIndex)
        return 4;
    else if (t[5]==edgeIndex)
        return 5;
    else
        return -1;
}

int MeshTopology::getTriangleIndexInTetrahedron(const TetraTriangles &t, TriangleID triangleIndex) const
{
    if (t[0]==triangleIndex)
        return 0;
    else if (t[1]==triangleIndex)
        return 1;
    else if (t[2]==triangleIndex)
        return 2;
    else if (t[3]==triangleIndex)
        return 3;
    else
        return -1;
}

int MeshTopology::getVertexIndexInHexahedron(Hexa &t, PointID vertexIndex) const
{
    if(t[0]==vertexIndex)
        return 0;
    else if(t[1]==vertexIndex)
        return 1;
    else if(t[2]==vertexIndex)
        return 2;
    else if(t[3]==vertexIndex)
        return 3;
    else if(t[4]==vertexIndex)
        return 4;
    else if(t[5]==vertexIndex)
        return 5;
    else if(t[6]==vertexIndex)
        return 6;
    else if(t[7]==vertexIndex)
        return 7;
    else
        return -1;
}

int MeshTopology::getEdgeIndexInHexahedron(const HexaEdges &t, EdgeID edgeIndex) const
{
    if(t[0]==edgeIndex)
        return 0;
    else if(t[1]==edgeIndex)
        return 1;
    else if(t[2]==edgeIndex)
        return 2;
    else if(t[3]==edgeIndex)
        return 3;
    else if(t[4]==edgeIndex)
        return 4;
    else if(t[5]==edgeIndex)
        return 5;
    else if(t[6]==edgeIndex)
        return 6;
    else if(t[7]==edgeIndex)
        return 7;
    else if(t[8]==edgeIndex)
        return 8;
    else if(t[9]==edgeIndex)
        return 9;
    else if(t[10]==edgeIndex)
        return 10;
    else if(t[11]==edgeIndex)
        return 11;
    else
        return -1;
}

int MeshTopology::getQuadIndexInHexahedron(const HexaQuads &t, QuadID quadIndex) const
{
    if(t[0]==quadIndex)
        return 0;
    else if(t[1]==quadIndex)
        return 1;
    else if(t[2]==quadIndex)
        return 2;
    else if(t[3]==quadIndex)
        return 3;
    else if(t[4]==quadIndex)
        return 4;
    else if(t[5]==quadIndex)
        return 5;
    else
        return -1;
}

MeshTopology::Edge MeshTopology::getLocalTetrahedronEdges (const unsigned int i) const
{
    assert(i<6);
    const unsigned int tetrahedronEdgeArray[6][2]= {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};
    return MeshTopology::Edge (tetrahedronEdgeArray[i][0], tetrahedronEdgeArray[i][1]);
}

bool MeshTopology::hasPos() const
{
    return !seqPoints.empty();
}

double MeshTopology::getPX(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][0]:0.0);
}

double MeshTopology::getPY(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][1]:0.0);
}

double MeshTopology::getPZ(int i) const
{
    return ((unsigned)i<seqPoints.size()?seqPoints[i][2]:0.0);
}

void MeshTopology::invalidate()
{
    validEdges = false;
    validTriangles = false;
    validQuads = false;
    validTetras = false;
    validHexas = false;
    m_edgeVertexShell.clear();
    m_edgeTriangleShell.clear();
    m_edgeQuadShell.clear();
    m_edgeTetraShell.clear();
    m_edgeHexaShell.clear();
    m_triangleVertexShell.clear();
    m_triangleEdgeShell.clear();
    m_triangleTetraShell.clear();
    m_quadVertexShell.clear();
    m_quadEdgeShell.clear();
    m_quadHexaShell.clear();
    m_tetraVertexShell.clear();
    m_tetraEdgeShell.clear();
    m_tetraTriangleShell.clear();
    m_hexaVertexShell.clear();
    m_hexaEdgeShell.clear();
    m_hexaQuadShell.clear();
    ++revision;
    //std::cout << "MeshTopology::invalidate()"<<std::endl;
}

void MeshTopology::updateEdges()
{
    if (!seqEdges.getValue().empty()) return; // edges already defined

    // create a temporary map to find redundant edges
    std::map<Edge,unsigned int> edgeMap;
    int edgeIndex;

    if (hasVolume())
    {
        const SeqTetras& tetras = getTetras(); // do not use seqTetras directly as it might not be up-to-date
        const unsigned int tetrahedronEdgeArray[6][2]= {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};
        for (unsigned int i = 0; i < tetras.size(); ++i)
        {
            const Tetra &t=tetras[i];
            std::map<Edge,unsigned int>::iterator ite;
            Edge e;
            unsigned int v1,v2;
            for (unsigned int j=0; j<6; ++j)
            {
                v1=t[tetrahedronEdgeArray[j][0]];
                v2=t[tetrahedronEdgeArray[j][1]];
                // sort vertices in lexicographics order
                if (v1<v2)
                    e=Edge(v1,v2);
                else
                    e=Edge(v2,v1);
            }
            ite=edgeMap.find(e);
            if (ite==edgeMap.end())
            {
                // edge not in edgeMap so create a new one
                edgeIndex=seqEdges.getValue().size();
                edgeMap[e]=edgeIndex;
                seqEdges.beginEdit()->push_back(e);
                seqEdges.endEdit();
            }
            else
            {
                edgeIndex=(*ite).second;
            }
            //m_edgeTetraShell[i][j]=edgeIndex;
        }

        const SeqHexas& hexas = getHexas(); // do not use seqHexas directly as it might not be up-to-date
        const unsigned int edgeHexahedronDescriptionArray[12][2]= {{0,1},{0,3},{0,4},{1,2},{1,5},{2,3},{2,6},{3,7},{4,5},{4,7},{5,6},{6,7}};
        // create a temporary map to find redundant edges
        std::map<Edge,unsigned int> edgeMap;
        /// create the m_edge array at the same time than it fills the m_hexahedronEdge array
        for (unsigned int i = 0; i < hexas.size(); ++i)
        {
            const Hexa &h=hexas[i];
            std::map<Edge,unsigned int>::iterator ite;
            Edge e;
            unsigned int v1,v2;
            for (unsigned int j=0; j<12; ++j)
            {
                v1=h[edgeHexahedronDescriptionArray[j][0]];
                v2=h[edgeHexahedronDescriptionArray[j][1]];
                // sort vertices in lexicographics order
                if (v1<v2)
                    e=Edge(v1,v2);
                else
                    e=Edge(v2,v1);
            }
            ite=edgeMap.find(e);
            if (ite==edgeMap.end())
            {
                // edge not in edgeMap so create a new one
                edgeIndex=seqEdges.getValue().size();
                edgeMap[e]=edgeIndex;
                seqEdges.beginEdit()->push_back(e);
                seqEdges.endEdit();
            }
            else
            {
                edgeIndex=(*ite).second;
            }
            //m_edgeHexaShell[i][j]=edgeIndex;
        }
    }
    else if (hasSurface())
    {

        const SeqTriangles& triangles = getTriangles(); // do not use seqTriangles directly as it might not be up-to-date
        for (unsigned int i = 0; i < triangles.size(); ++i)
        {
            const Triangle &t=triangles[i];
            std::map<Edge,unsigned int>::iterator ite;
            Edge e;
            unsigned int v1,v2;
            for (unsigned int j=0; j<3; ++j)
            {
                v1=t[(j+1)%3];
                v2=t[(j+2)%3];
                // sort vertices in lexicographics order
                if (v1<v2)
                    e=Edge(v1,v2);
                else
                    e=Edge(v2,v1);
                ite=edgeMap.find(e);
                if (ite==edgeMap.end())
                {
                    // edge not in edgeMap so create a new one
                    edgeIndex=seqEdges.getValue().size();
                    edgeMap[e]=edgeIndex;
                    seqEdges.beginEdit()->push_back(e);
                    seqEdges.endEdit();
                }
                else
                {
                    edgeIndex=(*ite).second;
                }
                //m_edgeTriangleShell[i][j]=edgeIndex;
            }
        }

        const SeqQuads& quads = getQuads(); // do not use seqQuads directly as it might not be up-to-date
        for (unsigned int i = 0; i < quads.size(); ++i)
        {
            const Quad &t=quads[i];
            std::map<Edge,unsigned int>::iterator ite;
            Edge e;
            unsigned int v1,v2;
            for (unsigned int j=0; j<4; ++j)
            {
                v1=t[(j+1)%4];
                v2=t[(j+2)%4];
                // sort vertices in lexicographics order
                if (v1<v2)
                    e=Edge(v1,v2);
                else
                    e=Edge(v2,v1);
                ite=edgeMap.find(e);
                if (ite==edgeMap.end())
                {
                    // edge not in edgeMap so create a new one
                    edgeIndex=seqEdges.getValue().size();
                    edgeMap[e]=edgeIndex;
                    seqEdges.beginEdit()->push_back(e);
                    seqEdges.endEdit();
                }
                else
                {
                    edgeIndex=(*ite).second;
                }
                //m_edgeQuadShell[i][j]=edgeIndex;
            }
        }
    }
}

void MeshTopology::updateQuads()
{
    if (!seqQuads.getValue().empty()) return; // quads already defined
    if (getNbHexas()==0) return; // no hexas to extract edges from

    const SeqHexas& hexas = getHexas(); // do not use seqQuads directly as it might not be up-to-date

    // create a temporary map to find redundant quads
    std::map<Quad,unsigned int> quadMap;
    std::map<Quad,unsigned int>::iterator itt;
    Quad qu;
    unsigned int v[4],val;
    int quadIndex;
    /// create the m_edge array at the same time than it fills the m_hexahedronEdge array
    for (unsigned int i = 0; i < hexas.size(); ++i)
    {
        const Hexa &h=hexas[i];

        // Quad 0 :
        v[0]=h[0]; v[1]=h[3]; v[2]=h[2]; v[3]=h[1];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;

            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();

        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][0]=quadIndex;

        // Quad 1 :
        v[0]=h[4]; v[1]=h[5]; v[2]=h[6]; v[3]=h[7];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;
            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();
        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][1]=quadIndex;

        // Quad 2 :
        v[0]=h[0]; v[1]=h[1]; v[2]=h[5]; v[3]=h[4];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;
            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();
        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][2]=quadIndex;

        // Quad 3 :
        v[0]=h[1]; v[1]=h[2]; v[2]=h[6]; v[3]=h[5];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;
            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();
        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][3]=quadIndex;

        // Quad 4 :
        v[0]=h[2]; v[1]=h[3]; v[2]=h[7]; v[3]=h[6];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;
            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();
        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][4]=quadIndex;

        // Quad 5 :
        v[0]=h[3]; v[1]=h[0]; v[2]=h[4]; v[3]=h[7];
        // sort v such that v[0] is the smallest one
        while ((v[0]>v[1]) || (v[0]>v[2]) || (v[0]>v[3]))
        {
            val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=v[3]; v[3]=val;
        }
        //std::sort(v,v+1); std::sort(v,v+2); std::sort(v,v+3);
        //std::sort(v+1,v+2); std::sort(v+1,v+3);
        //std::sort(v+2,v+3);
        // sort vertices in lexicographics order
        qu=helper::make_array<unsigned int>(v[0],v[3],v[2],v[1]);
        itt=quadMap.find(qu);
        if (itt==quadMap.end())
        {
            // quad not in edgeMap so create a new one
            quadIndex=seqQuads.getValue().size();
            quadMap[qu]=quadIndex;
            qu=helper::make_array<unsigned int>(v[0],v[1],v[2],v[3]);
            quadMap[qu]=quadIndex;
            seqQuads.beginEdit()->push_back(qu);
            seqQuads.endEdit();
        }
        else
        {
            quadIndex=(*itt).second;
        }
        //m_quadHexaShell[i][5]=quadIndex;
    }
}

void MeshTopology::updateTriangles()
{
    if (!seqTriangles.getValue().empty()) return; // triangles already defined
    if (getNbTetras()==0) return; // no tetras to extract edges from
    const SeqTetras& tetras = getTetras(); // do not use seqTriangles directly as it might not be up-to-date

    // create a temporary map to find redundant triangles
    std::map<Triangle,unsigned int> triangleMap;
    std::map<Triangle,unsigned int>::iterator itt;
    Triangle tr;
    int triangleIndex;
    unsigned int v[3],val;
    /// create the m_edge array at the same time than it fills the m_triangleTetraShell array
    for (unsigned int i = 0; i < tetras.size(); ++i)
    {
        const Tetra &t=seqTetras.getValue()[i];
        for (unsigned int j=0; j<4; ++j)
        {
            if (j%2)
            {
                v[0]=t[(j+1)%4]; v[1]=t[(j+2)%4]; v[2]=t[(j+3)%4];
            }
            else
            {
                v[0]=t[(j+1)%4]; v[2]=t[(j+2)%4]; v[1]=t[(j+3)%4];
            }
            //		std::sort(v,v+2);
            // sort v such that v[0] is the smallest one
            while ((v[0]>v[1]) || (v[0]>v[2]))
            {
                val=v[0]; v[0]=v[1]; v[1]=v[2]; v[2]=val;
            }
            // check if a triangle with an opposite orientation already exists
            tr=helper::make_array<unsigned int>(v[0],v[2],v[1]);
            itt=triangleMap.find(tr);
            if (itt==triangleMap.end())
            {
                // edge not in edgeMap so create a new one
                triangleIndex=seqTriangles.getValue().size();
                tr=helper::make_array<unsigned int>(v[0],v[1],v[2]);
                triangleMap[tr]=triangleIndex;
                seqTriangles.beginEdit()->push_back(tr);
                seqTriangles.endEdit();
            }
            else
            {
                triangleIndex=(*itt).second;
            }
            //m_triangleTetraShell[i][j]=triangleIndex;
        }
    }
}

void MeshTopology::updateHexas()
{
    if (!seqHexas.getValue().empty()) return; // hexas already defined
    // No 4D elements yet! ;)
}

void MeshTopology::updateTetras()
{
    if (!seqTetras.getValue().empty()) return; // tetras already defined
    // No 4D elements yet! ;)
}

void MeshTopology::draw()
{
    if (_draw.getValue())
    {
        glDisable(GL_LIGHTING);


        glColor3f(1,0,0);
        for (int i=0; i<getNbHexas(); i++)
        {
            const Hexa& c = getHexa(i);
            glBegin(GL_LINE_STRIP);
            glVertex3d(getPX(c[0]), getPY(c[0]), getPZ(c[0]));
            glVertex3d(getPX(c[1]), getPY(c[1]), getPZ(c[1]));
            glVertex3d(getPX(c[2]), getPY(c[2]), getPZ(c[2]));
            glVertex3d(getPX(c[3]), getPY(c[3]), getPZ(c[3]));
            glVertex3d(getPX(c[0]), getPY(c[0]), getPZ(c[0]));
            glEnd();
            glBegin(GL_LINE_STRIP);
            glVertex3d(getPX(c[4]), getPY(c[4]), getPZ(c[4]));
            glVertex3d(getPX(c[5]), getPY(c[5]), getPZ(c[5]));
            glVertex3d(getPX(c[6]), getPY(c[6]), getPZ(c[6]));
            glVertex3d(getPX(c[7]), getPY(c[7]), getPZ(c[7]));
            glVertex3d(getPX(c[4]), getPY(c[4]), getPZ(c[4]));
            glEnd();
            glBegin(GL_LINES);
            glVertex3d(getPX(c[3]), getPY(c[3]), getPZ(c[3]));
            glVertex3d(getPX(c[7]), getPY(c[7]), getPZ(c[7]));
            //glEnd();
            //glBegin(GL_LINES);
            glVertex3d(getPX(c[2]), getPY(c[2]), getPZ(c[2]));
            glVertex3d(getPX(c[6]), getPY(c[6]), getPZ(c[6]));
            //glEnd();
            //glBegin(GL_LINES);
            glVertex3d(getPX(c[0]), getPY(c[0]), getPZ(c[0]));
            glVertex3d(getPX(c[4]), getPY(c[4]), getPZ(c[4]));
            //glEnd();
            //glBegin(GL_LINES);
            glVertex3d(getPX(c[1]), getPY(c[1]), getPZ(c[1]));
            glVertex3d(getPX(c[5]), getPY(c[5]), getPZ(c[5]));
            glEnd();
        }
    }
}

} // namespace topology

} // namespace component

} // namespace sofa

