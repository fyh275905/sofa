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
#include <SofaMiscTopology/TopologyChecker.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/ObjectFactory.h>

#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/simulation/AnimateEndEvent.h>


#include <sofa/simulation/Simulation.h>

namespace sofa::component::misc
{

using namespace defaulttype;
using namespace sofa::core::topology;


int TopologyCheckerClass = core::RegisterObject("Read topological Changes and process them.")
        .add< TopologyChecker >();


TopologyChecker::TopologyChecker()
    : m_draw( initData(&m_draw, false, "draw", "draw information"))
    , d_eachStep(initData(&d_eachStep, false, "draw", "Check topology at each step"))
    , l_topology(initLink("topology", "link to the topology container"))
    , m_topology(nullptr)
{
    this->f_listening.setValue(true);
}


TopologyChecker::~TopologyChecker()
{

}


void TopologyChecker::init()
{
    if (l_topology.empty())
    {
        msg_info() << "link to Topology container should be set to ensure right behavior. First Topology found in current context will be used.";
        l_topology.set(this->getContext()->getMeshTopologyLink());
    }

    m_topology = l_topology.get();
    msg_info() << "Topology path used: '" << l_topology.getLinkedPath() << "'";

    if (m_topology == nullptr)
    {
        msg_error() << "No topology component found at path: " << l_topology.getLinkedPath() << ", nor in current context: " << this->getContext()->name;
        sofa::core::objectmodel::BaseObject::d_componentState.setValue(sofa::core::objectmodel::ComponentState::Invalid);
        return;
    }


    checkContainer();
}

void TopologyChecker::reinit()
{

}


bool TopologyChecker::checkContainer()
{
    bool result = false;
    if (m_topology->getTopologyType() == TopologyElementType::TETRAHEDRON)
        result = checkTetrahedronTopology();
    else if (m_topology->getTopologyType() == TopologyElementType::QUAD)
        result = checkTriangleTopology();
    else if (m_topology->getTopologyType() == TopologyElementType::TRIANGLE)
        result = checkQuadTopology();
    else if (m_topology->getTopologyType() == TopologyElementType::EDGE)
        result = checkEdgeTopology();

    return result;
}


bool TopologyChecker::checkEdgeTopology()
{
    std::cout << "TopologyChecker::checkEdgeTopology()" << std::endl;
    bool ret = true;
    int nbE = m_topology->getNbEdges();
    const sofa::core::topology::BaseMeshTopology::SeqEdges& my_edges = m_topology->getEdges();

    if (nbE != my_edges.size())
    {
        msg_error() << "CheckEdgeTopology failed: not the good number of edges, getNbEdges returns " << nbE << " whereas edge array size is: " << my_edges.size();
        return false;
    }


    // check edge buffer
    for (std::size_t i = 0; i < nbE; ++i)
    {
        const auto& edge = my_edges[i];
        if (edge[0] == edge[1]) {
            msg_error() << "CheckEdgeTopology failed: edge " << i << " has 2 identical vertices: " << edge;
            ret = false;
        }
    }

    // check cross element
    std::size_t nbP = m_topology->getNbPoints();

    std::set<int> edgeSet;
    for (std::size_t i = 0; i < nbP; ++i)
    {
        const auto& EdgesAV = m_topology->getEdgesAroundVertex(i);
        for (size_t j = 0; j < EdgesAV.size(); ++j)
        {
            const Topology::Edge& edge = my_edges[EdgesAV[j]];
            if (!(edge[0] == i || edge[1] == i))
            {
                msg_error() << "CheckEdgeTopology failed: edge " << EdgesAV[j] << ": [" << edge << "] not around vertex: " << i;
                ret = false;
            }

            // count number of edge
            edgeSet.insert(EdgesAV[j]);
        }
    }

    if (edgeSet.size() != nbE)
    {
        msg_error() << "CheckEdgeTopology failed: found " << edgeSet.size() << " edges in m_edgesAroundVertex out of " << nbE;
        ret = false;
    }

    return ret;
}


bool TopologyChecker::checkTriangleTopology()
{
    std::cout << "TopologyChecker::checkTriangleTopology()" << std::endl;
    bool ret = true;
    int nbT = m_topology->getNbTriangles();
    const sofa::core::topology::BaseMeshTopology::SeqTriangles& my_triangles = m_topology->getTriangles();

    if (nbT != my_triangles.size())
    {
        msg_error() << "CheckTriangleTopology failed: not the good number of triangles, getNbTriangles returns " << nbT << " whereas triangle array size is: " << my_triangles.size();
        return false;
    }

    // check triangle buffer
    for (std::size_t i = 0; i < nbT; ++i)
    {
        const auto& triangle = my_triangles[i];
        if (triangle[0] == triangle[1] || triangle[0] == triangle[2] || triangle[1] == triangle[2]) {
            msg_error() << "CheckTriangleTopology failed: triangle " << i << " has 2 identical vertices: " << triangle;
            ret = false;
        }
    }

    // check cross element
    std::size_t nbP = m_topology->getNbPoints();

    // check triangles around vertex
    std::set <int> triangleSet;
    for (std::size_t i = 0; i < nbP; ++i)
    {
        const auto& triAV = m_topology->getTrianglesAroundVertex(i);
        for (size_t j = 0; j < triAV.size(); ++j)
        {
            const Topology::Triangle& triangle = my_triangles[triAV[j]];
            bool check_triangle_vertex_shell = (triangle[0] == i) || (triangle[1] == i) || (triangle[2] == i);
            if (!check_triangle_vertex_shell)
            {
                msg_error() << "CheckTriangleTopology failed: triangle " << triAV[j] << ": [" << triangle << "] not around vertex: " << i;
                ret = false;
            }

            triangleSet.insert(triAV[j]);
        }
    }

    if (triangleSet.size() != my_triangles.size())
    {
        msg_error() << "CheckTriangleTopology failed: found " << triangleSet.size() << " triangles in trianglesAroundVertex out of " << my_triangles.size();
        ret = false;
    }
    

    int nbE = m_topology->getNbEdges();
    const sofa::core::topology::BaseMeshTopology::SeqEdges& my_edges = m_topology->getEdges();
    // check edges in triangles
    for (std::size_t i = 0; i < nbT; ++i)
    {
        const Topology::Triangle& triangle = my_triangles[i];
        const auto& eInTri = m_topology->getEdgesInTriangle(i);

        for (unsigned int j = 0; j < 3; j++)
        {
            const Topology::Edge& edge = my_edges[eInTri[j]];
            int cptFound = 0;
            for (unsigned int k = 0; k < 3; k++)
                if (edge[0] == triangle[k] || edge[1] == triangle[k])
                    cptFound++;

            if (cptFound != 2)
            {
                msg_error() << "CheckTriangleTopology failed: edge: " << eInTri[j] << ": [" << edge << "] not found in triangle: " << i << ": " << triangle;
                ret = false;
            }
        }
    }
    
    // check triangles around edges
    // check m_trianglesAroundEdge using checked m_edgesInTriangle
    triangleSet.clear();
    for (size_t edgeId = 0; edgeId < nbE; ++edgeId)
    {
        const BaseMeshTopology::TrianglesAroundEdge& tes = m_topology->getTrianglesAroundEdge(edgeId);
        for (auto triId : tes)
        {
            const BaseMeshTopology::EdgesInTriangle& eInTri = m_topology->getEdgesInTriangle(triId);
            bool check_triangle_edge_shell = (eInTri[0] == edgeId)
                || (eInTri[1] == edgeId)
                || (eInTri[2] == edgeId);
            if (!check_triangle_edge_shell)
            {
                msg_error() << "CheckTriangleTopology failed: triangle: " << triId << " with edges: [" << eInTri << "] not found around edge: " << edgeId;
                ret = false;
            }

            triangleSet.insert(triId);
        }
    }

    if (triangleSet.size() != my_triangles.size())
    {
        msg_error() << "CheckTriangleTopology failed: found " << triangleSet.size() << " triangles in m_trianglesAroundEdge out of " << my_triangles.size();
        ret = false;
    }

    return ret && checkEdgeTopology();
}


bool TopologyChecker::checkQuadTopology()
{
    std::cout << "TopologyChecker::checkQuadTopology()" << std::endl;
    bool ret = true;
    int nbQ = m_topology->getNbQuads();
    const sofa::core::topology::BaseMeshTopology::SeqQuads& my_quads = m_topology->getQuads();

    if (nbQ != my_quads.size())
    {
        msg_error() << "checkQuadTopology failed: not the good number of quads, getNbQuads returns " << nbQ << " whereas quad array size is: " << my_quads.size();
        return false;
    }

    // check triangle buffer
    for (std::size_t i = 0; i < nbQ; ++i)
    {
        const auto& quad = my_quads[i];
        for (int j = 0; j < 3; ++j)
        {
            for (int k = j + 1; k < 4; ++k)
            {
                if (quad[j] == quad[k])
                {
                    msg_error() << "checkQuadTopology failed: quad " << i << " has 2 identical vertices: " << quad;
                    ret = false;
                }
            }
        }
    }

    // check cross element
    std::size_t nbP = m_topology->getNbPoints();

    // check quads around vertex
    std::set <int> quadSet;
    for (std::size_t i = 0; i < nbP; ++i)
    {
        const auto& quadAV = m_topology->getQuadsAroundVertex(i);
        for (size_t j = 0; j < quadAV.size(); ++j)
        {
            const Topology::Quad& quad = my_quads[quadAV[j]];
            bool check_quad_vertex_shell = (quad[0] == i) || (quad[1] == i) || (quad[2] == i) || (quad[3] == i);
            if (!check_quad_vertex_shell)
            {
                msg_error() << "CheckQuadTopology failed: quad " << quadAV[j] << ": [" << quad << "] not around vertex: " << i;
                ret = false;
            }

            quadSet.insert(quadAV[j]);
        }
    }

    if (quadSet.size() != my_quads.size())
    {
        msg_error() << "CheckQuadTopology failed: found " << quadSet.size() << " quads in quadsAroundVertex out of " << my_quads.size();
        ret = false;
    }


    int nbE = m_topology->getNbEdges();
    const sofa::core::topology::BaseMeshTopology::SeqEdges& my_edges = m_topology->getEdges();
    // check edges in quads
    for (std::size_t i = 0; i < nbQ; ++i)
    {
        const Topology::Quad& quad = my_quads[i];
        const auto& eInQ = m_topology->getEdgesInQuad(i);

        for (auto eId : eInQ)
        {
            const Topology::Edge& edge = my_edges[eId];
            int cptFound = 0;
            for (unsigned int k = 0; k < 4; k++)
                if (edge[0] == quad[k] || edge[1] == quad[k])
                    cptFound++;

            if (cptFound != 2)
            {
                msg_error() << "CheckQuadTopology failed: edge: " << eId << ": [" << edge << "] not found in quad: " << i << ": " << quad;
                ret = false;
            }
        }
    }


    // check quads around edges
    // check m_quadsAroundEdge using checked m_edgesInQuad
    quadSet.clear();
    for (size_t edgeId = 0; edgeId < nbE; ++edgeId)
    {
        const BaseMeshTopology::QuadsAroundEdge& qAE = m_topology->getQuadsAroundEdge(edgeId);
        for (auto qId : qAE)
        {
            const BaseMeshTopology::EdgesInQuad& eInQ = m_topology->getEdgesInQuad(qId);
            bool check_quad_edge_shell = (eInQ[0] == edgeId)
                || (eInQ[1] == edgeId)
                || (eInQ[2] == edgeId)
                || (eInQ[3] == edgeId);
            if (!check_quad_edge_shell)
            {
                msg_error() << "CheckQuadTopology failed: quad: " << qId << " with edges: [" << eInQ << "] not found around edge: " << edgeId;
                ret = false;
            }

            quadSet.insert(qId);
        }
    }

    if (quadSet.size() != my_quads.size())
    {
        msg_error() << "CheckQuadTopology failed: found " << quadSet.size() << " quads in m_quadsAroundEdge out of " << my_quads.size();
        ret = false;
    }
    return ret && checkEdgeTopology();
}


bool TopologyChecker::checkTetrahedronTopology()
{
    std::cout << "TopologyChecker::checkTetrahedronTopology()" << std::endl;
    bool ret = true;
    int nbT = m_topology->getNbTetrahedra();
    const sofa::core::topology::BaseMeshTopology::SeqTetrahedra& my_tetrahedra = m_topology->getTetrahedra();

    if (nbT != my_tetrahedra.size())
    {
        msg_error() << "checkTetrahedronTopology failed: not the good number of tetrahedra, getNbTetrahedra returns " << nbT << " whereas triangle array size is: " << my_tetrahedra.size();
        return false;
    }

    // check tetrahedron buffer
    for (std::size_t i = 0; i < nbT; ++i)
    {
        const auto& tetra = my_tetrahedra[i];
        for (int j = 0; j < 3; ++j)
        {
            for (int k = j + 1; k < 4; ++k)
            {
                if (tetra[j] == tetra[k])
                {
                    msg_error() << "checkTetrahedronTopology failed: tetrahedron " << i << " has 2 identical vertices: " << tetra;
                    ret = false;
                }
            }
        }
    }

    // check cross element
    std::size_t nbP = m_topology->getNbPoints();

    // check tetrahedra around vertex
    std::set <int> tetrahedronSet;
    for (std::size_t pId = 0; pId < nbP; ++pId)
    {
        const auto& tetraAV = m_topology->getTetrahedraAroundVertex(pId);
        for (auto tetraId : tetraAV )
        {
            const Topology::Tetrahedron& tetra = my_tetrahedra[tetraId];
            bool check_tetra_vertex_shell = (tetra[0] == pId)
                || (tetra[1] == pId)
                || (tetra[2] == pId)
                || (tetra[3] == pId);
            if (!check_tetra_vertex_shell)
            {
                msg_error() << "checkTetrahedronTopology failed: Tetrahedron " << tetraId << ": [" << tetra << "] not around vertex: " << pId;
                ret = false;
            }

            tetrahedronSet.insert(tetraId);
        }
    }

    if (tetrahedronSet.size() != my_tetrahedra.size())
    {
        msg_error() << "checkTetrahedronTopology failed: found " << tetrahedronSet.size() << " tetrahedra in tetrahedraAroundVertex out of " << my_tetrahedra.size();
        ret = false;
    }



    int nbTri = m_topology->getNbTriangles();
    const sofa::core::topology::BaseMeshTopology::SeqTriangles& my_triangles = m_topology->getTriangles();
    // check first m_trianglesInTetrahedron
    for (std::size_t tetraId = 0; tetraId < nbT; ++tetraId)
    {
        const Topology::Tetrahedron& tetrahedron = my_tetrahedra[tetraId];
        const auto& triInTetra = m_topology->getTrianglesInTetrahedron(tetraId);

        for (unsigned int j = 0; j < 4; j++)
        {
            const Topology::Triangle& triangle = my_triangles[triInTetra[j]];
            int cptFound = 0;
            for (unsigned int k = 0; k < 4; k++)
                if (triangle[0] == tetrahedron[k] || triangle[1] == tetrahedron[k] || triangle[2] == tetrahedron[k])
                    cptFound++;

            if (cptFound != 3)
            {
                msg_error() << "checkTetrahedronTopology failed: triangle: " << triInTetra[j] << ": [" << triangle << "] not found in tetrahedron: " << tetraId << ": " << tetrahedron;
                ret = false;
            }
        }
    }

    // check tetrahedra around triangles
    // check m_tetrahedraAroundTriangle using checked m_trianglesInTetrahedron
    tetrahedronSet.clear();
    for (size_t triId = 0; triId < nbTri; ++triId)
    {
        const BaseMeshTopology::TetrahedraAroundTriangle& tes = m_topology->getTetrahedraAroundTriangle(triId);
        for (auto tetraId : tes)
        {
            const BaseMeshTopology::TrianglesInTetrahedron& triInTetra = m_topology->getTrianglesInTetrahedron(tetraId);
            bool check_tetra_triangle_shell = (triInTetra[0] == triId)
                || (triInTetra[1] == triId)
                || (triInTetra[2] == triId)
                || (triInTetra[3] == triId);
            if (!check_tetra_triangle_shell)
            {
                msg_error() << "checkTetrahedronTopology failed: tetrahedron: " << tetraId << " with triangle: [" << triInTetra << "] not found around triangle: " << tetraId;
                ret = false;
            }

            tetrahedronSet.insert(tetraId);
        }
    }

    if (tetrahedronSet.size() != my_tetrahedra.size())
    {
        msg_error() << "checkTetrahedronTopology failed: found " << tetrahedronSet.size() << " tetrahedra in m_tetrahedraAroundTriangle out of " << my_tetrahedra.size();
        ret = false;
    }



    int nbE = m_topology->getNbEdges();
    const sofa::core::topology::BaseMeshTopology::SeqEdges& my_edges = m_topology->getEdges();
    // check edges in tetrahedra
    for (std::size_t i = 0; i < nbT; ++i)
    {
        const Topology::Tetrahedron& tetrahedron = my_tetrahedra[i];
        const auto& eInTetra = m_topology->getEdgesInTetrahedron(i);

        for (unsigned int j = 0; j < 6; j++)
        {
            const Topology::Edge& edge = my_edges[eInTetra[j]];
            int cptFound = 0;
            for (unsigned int k = 0; k < 4; k++)
                if (edge[0] == tetrahedron[k] || edge[1] == tetrahedron[k])
                    cptFound++;

            if (cptFound != 2)
            {
                msg_error() << "checkTetrahedronTopology failed: edge: " << eInTetra[j] << ": [" << edge << "] not found in tetrahedron: " << i << ": " << tetrahedron;
                ret = false;
            }
        }
    }

    // check tetrahedra around edges
    // check m_tetrahedraAroundEdge using checked m_edgesInTetrahedron
    tetrahedronSet.clear();
    for (size_t edgeId = 0; edgeId < nbE; ++edgeId)
    {
        const BaseMeshTopology::TetrahedraAroundEdge& tes = m_topology->getTetrahedraAroundEdge(edgeId);
        for (auto tetraId : tes)
        {
            const BaseMeshTopology::EdgesInTetrahedron& eInTetra = m_topology->getEdgesInTetrahedron(tetraId);
            bool check_tetra_edge_shell = (eInTetra[0] == edgeId)
                || (eInTetra[1] == edgeId)
                || (eInTetra[2] == edgeId)
                || (eInTetra[3] == edgeId)
                || (eInTetra[4] == edgeId)
                || (eInTetra[5] == edgeId);
            if (!check_tetra_edge_shell)
            {
                msg_error() << "checkTetrahedronTopology failed: tetrahedron: " << tetraId << " with edges: [" << eInTetra << "] not found around edge: " << edgeId;
                ret = false;
            }

            tetrahedronSet.insert(tetraId);
        }
    }

    if (tetrahedronSet.size() != my_tetrahedra.size())
    {
        msg_error() << "CheckTriangleTopology failed: found " << tetrahedronSet.size() << " tetrahedra in m_tetrahedraAroundTriangle out of " << my_tetrahedra.size();
        ret = false;
    }


    return ret && checkTriangleTopology();
}


void TopologyChecker::checkCrossTopology()
{

}



void TopologyChecker::handleEvent(sofa::core::objectmodel::Event* event)
{
    if (sofa::core::objectmodel::KeypressedEvent* ev = dynamic_cast<sofa::core::objectmodel::KeypressedEvent*>(event))
    {
        if (ev->getKey() == 'D')
        {
            bool res = checkContainer();
            if (!res)
                msg_error() << "CheckContainer Error!!";
        }
    }

    if (simulation::AnimateEndEvent::checkEventType(event) && d_eachStep.getValue())
    {
        
    }
}


void TopologyChecker::draw(const core::visual::VisualParams* vparams)
{
    if (!m_topology)
        return;

    if(!m_draw.getValue())
        return;

    if (!vparams->displayFlags().getShowBehaviorModels())
        return;

    //sofa::component::topology::TriangleSetGeometryAlgorithms<Vec3Types>* triangleGeo;
    //m_topology->getContext()->get(triangleGeo);

    //if (!triangleGeo)
    //    return;

    //size_t nbTriangles = m_topology->getNbTriangles();

    //std::vector< Vector3 > trianglesToDraw;
    //std::vector< Vector3 > pointsToDraw;

    //for (size_t i = 0 ; i < triangleIncisionInformation.size() ; i++)
    //{
    //    for (size_t j = 0 ; j < triangleIncisionInformation[i].triangleIndices.size() ; j++)
    //    {
    //        unsigned int triIndex = triangleIncisionInformation[i].triangleIndices[j];

    //        if ( triIndex > nbTriangles -1)
    //            break;

    //        Vec3Types::Coord coord[3];
    //        triangleGeo->getTriangleVertexCoordinates(triIndex, coord);

    //        for(unsigned int k = 0 ; k < 3 ; k++)
    //            trianglesToDraw.push_back(coord[k]);

    //        Vector3 a;
    //        a.clear();
    //        for (unsigned k = 0 ; k < 3 ; k++)
    //            a += coord[k] * triangleIncisionInformation[i].barycentricCoordinates[j][k];

    //        pointsToDraw.push_back(a);
    //    }
    //}

    //vparams->drawTool()->drawTriangles(trianglesToDraw, Vec<4,float>(0.0,0.0,1.0,1.0));
    //vparams->drawTool()->drawPoints(pointsToDraw, 15.0,  Vec<4,float>(1.0,0.0,1.0,1.0));

    //if (!errorTrianglesIndices.empty())
    //{
    //    trianglesToDraw.clear();
    //    /* initialize random seed: */
    //    srand ( (unsigned int)time(nullptr) );

    //    for (size_t i = 0 ; i < errorTrianglesIndices.size() ; i++)
    //    {
    //        Vec3Types::Coord coord[3];
    //        triangleGeo->getTriangleVertexCoordinates(errorTrianglesIndices[i], coord);

    //        for(unsigned int k = 0 ; k < 3 ; k++)
    //            trianglesToDraw.push_back(coord[k]);
    //    }

    //    vparams->drawTool()->drawTriangles(trianglesToDraw,
    //            Vec<4,float>(1.0f,(float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, 1.0f));
    //}
}

} // namespace misc

} // namespace component

} // namespace sofa
