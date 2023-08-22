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
#pragma once
#include <sofa/component/topology/container/dynamic/config.h>
#include <sofa/component/topology/container/dynamic/PointModifiers.h>

#include <sofa/type/vector.h>
#include <sofa/core/topology/BaseTopology.h>

namespace sofa::component::topology::container::dynamic
{

using TriangleID = core::topology::BaseMeshTopology::TriangleID;
using EdgeID = core::topology::BaseMeshTopology::EdgeID;


class TriangleToAdd
{
public:
    TriangleToAdd(TriangleID uniqueID, sofa::core::topology::BaseMeshTopology::Triangle _triangle,
        const type::vector<TriangleID>& ancestors, const type::vector<SReal>& coefs)
        : m_uniqueID(uniqueID)
        , m_triangle(_triangle)
        , m_ancestors(ancestors)
        , m_coefs(coefs)
    {}

    virtual ~TriangleToAdd() {}

    TriangleID m_uniqueID;
    sofa::core::topology::BaseMeshTopology::Triangle m_triangle;
    type::vector<TriangleID> m_ancestors;
    type::vector<SReal> m_coefs;
};


class TriangleToSplit
{
public:
    TriangleToSplit(TriangleID triangleId, const sofa::core::topology::BaseMeshTopology::Triangle& triangle)
        : m_triangleId(triangleId)
        , m_triangle(triangle)
    {}

    virtual ~TriangleToSplit() {}

    int localVertexId(PointID vertexIndex) 
    {
        for (unsigned int i = 0; i < 3; ++i)
        {
            if (m_triangle[i] == vertexIndex)
                return i;
        }

        return InvalidID;
    }

    TriangleID m_triangleId;
    sofa::core::topology::BaseMeshTopology::Triangle m_triangle;
    type::vector<PointToAdd*> m_points;
};


class TriangleSubdivider
{
public:
    using Triangle = sofa::core::topology::BaseMeshTopology::Triangle;
    using Edge = sofa::core::topology::BaseMeshTopology::Edge;

    TriangleSubdivider(TriangleToSplit* _triangleToSplit)
        : m_triangleToSplit(_triangleToSplit)
    {}

    virtual bool subdivide(const sofa::type::Vec3& ptA, const sofa::type::Vec3& ptB, const sofa::type::Vec3& ptC) 
    {
        SOFA_UNUSED(ptA);
        SOFA_UNUSED(ptB);
        SOFA_UNUSED(ptC);
        return false;
    }

    TriangleToSplit* m_triangleToSplit;
    type::vector<TriangleToAdd*> m_trianglesToAdd;
};


class TriangleSubdivider_1Node : public TriangleSubdivider
{
public:
    TriangleSubdivider_1Node(TriangleToSplit* _triangleToSplit) : TriangleSubdivider(_triangleToSplit) {}

    bool subdivide(const sofa::type::Vec3& ptA, const sofa::type::Vec3& ptB, const sofa::type::Vec3& ptC) override
    {
        if (m_triangleToSplit->m_points.size() != 1)
        {
            msg_error("TriangleSubdivider_1Node") << "More than 1 point to add to subdivide triangle id: " << m_triangleToSplit->m_triangleId;
            return false;
        }

        const PointToAdd* PTA = m_triangleToSplit->m_points[0];
         
        type::vector<TriangleID> ancestors;
        ancestors.push_back(m_triangleToSplit->m_triangleId);
        type::vector<SReal> coefs;
        coefs.push_back(0.3333); // 3 new triangles (need to compute real area proportion)

        for (unsigned int i = 0; i < 3; i++)
        {
            TriangleID uniqID = 1000000 * m_triangleToSplit->m_triangleId + i;
            Triangle newTri = Triangle(m_triangleToSplit->m_triangle[i], m_triangleToSplit->m_triangle[(i+1)%3], PTA->m_idPoint);
            auto TTA = new TriangleToAdd(uniqID, newTri, ancestors, coefs);
            m_trianglesToAdd.push_back(TTA);
        }

        return true;
    }
};


class TriangleSubdivider_1Edge : public TriangleSubdivider
{
public:
    TriangleSubdivider_1Edge(TriangleToSplit* _triangleToSplit, EdgeID localEdgeId)
        : TriangleSubdivider(_triangleToSplit)
        , m_localEdgeId(localEdgeId)
    {}

    EdgeID m_localEdgeId;

    bool subdivide(const sofa::type::Vec3& ptA, const sofa::type::Vec3& ptB, const sofa::type::Vec3& ptC) override
    {
        if (m_triangleToSplit->m_points.size() != 1)
        {
            msg_error("TriangleSubdivider_1Node") << "More than 1 point to add to subdivide triangle id: " << m_triangleToSplit->m_triangleId;
            return false;
        }

        const PointToAdd* PTA = m_triangleToSplit->m_points[0];

        type::vector<TriangleID> ancestors;
        ancestors.push_back(m_triangleToSplit->m_triangleId);
        type::vector<SReal> coefs;
        coefs.push_back(0.5); // 2 new triangles (need to compute real area proportion)

        Triangle newTri0 = Triangle(m_triangleToSplit->m_triangle[(m_localEdgeId + 1) % 3], PTA->m_idPoint, m_triangleToSplit->m_triangle[m_localEdgeId]);
        Triangle newTri1 = Triangle(PTA->m_idPoint, m_triangleToSplit->m_triangle[(m_localEdgeId + 2) % 3], m_triangleToSplit->m_triangle[m_localEdgeId]);
        
        auto TTA0 = new TriangleToAdd(1000000 * m_triangleToSplit->m_triangleId + 1, newTri0, ancestors, coefs);
        auto TTA1 = new TriangleToAdd(1000000 * m_triangleToSplit->m_triangleId + 2, newTri1, ancestors, coefs);

        m_trianglesToAdd.push_back(TTA0);
        m_trianglesToAdd.push_back(TTA1);

        return true;
    }

};


class TriangleSubdivider_2Edge : public TriangleSubdivider
{
public:
    TriangleSubdivider_2Edge(TriangleToSplit* _triangleToSplit, EdgeID localEdgeId0, EdgeID localEdgeId1)
        : TriangleSubdivider(_triangleToSplit) 
    {
        if (localEdgeId0 < localEdgeId1)
        {
            m_localEdgeId0 = localEdgeId0;
            m_localEdgeId1 = localEdgeId1;
        }
        else
        {
            m_localEdgeId0 = localEdgeId1;
            m_localEdgeId1 = localEdgeId0;
        }
    }


    bool subdivide(const sofa::type::Vec3& ptA, const sofa::type::Vec3& ptB, const sofa::type::Vec3& ptC) override
    {
        if (m_triangleToSplit->m_points.size() != 2)
        {
            msg_error("TriangleSubdivider_1Node") << "There are no 2 points to add to subdivide triangle id: " << m_triangleToSplit->m_triangleId;
            return false;
        }

        const PointToAdd* PTA0 = m_triangleToSplit->m_points[0];
        const PointToAdd* PTA1 = m_triangleToSplit->m_points[1];

        // get commun point
        const Edge theEdge0 = Edge(PTA0->m_ancestors[0], PTA0->m_ancestors[1]);
        const Edge theEdge1 = Edge(PTA1->m_ancestors[0], PTA1->m_ancestors[1]);

        PointID communLocalID = InvalidID;
        PointID communID = InvalidID;
        Edge theLocalEdge0;
        Edge theLocalEdge1;
        for (unsigned int i = 0; i < 3; i++)
        {
            auto pointId = m_triangleToSplit->m_triangle[i];
            unsigned int cpt = 0;

            if (pointId == theEdge0[0]) 
            {
                theLocalEdge0[0] = i;
                cpt++;
            }

            if (pointId == theEdge0[1])
            {
                theLocalEdge0[1] = i;
                cpt++;
            }


            if (pointId == theEdge1[0])
            {
                theLocalEdge1[0] = i;
                cpt++;
            }

            if (pointId == theEdge1[1])
            {
                theLocalEdge1[1] = i;
                cpt++;
            }

            if (cpt == 2)
            {
                communLocalID = i;
                communID = pointId;
            }
        }

        PointID nextID = m_triangleToSplit->m_triangle[(communLocalID + 1) % 3];
        PointID otherEdgeId0 = (theEdge0[0] == communID) ? theEdge0[1] : theEdge0[0];
        PointID otherEdgeId1 = (theEdge1[0] == communID) ? theEdge1[1] : theEdge1[0];

        bool directOriented = (otherEdgeId1 == nextID) ? true : false;

        type::vector<TriangleID> ancestors;
        ancestors.push_back(m_triangleToSplit->m_triangleId);
        type::vector<SReal> coefs;
        coefs.push_back(0.3333); // 3 new triangles (need to compute real area proportion)

        sofa::type::fixed_array<sofa::type::Vec3, 3> triPoints = { ptA , ptB, ptC };
        sofa::type::fixed_array<PointID, 4> baseQuadriID;
        sofa::type::fixed_array<sofa::type::Vec3, 4> quadPoints;
       
        sofa::type::Vec3 p0 = triPoints[theLocalEdge0[0]] * PTA0->m_coefs[0] + triPoints[theLocalEdge0[1]] * PTA0->m_coefs[1];
        sofa::type::Vec3 p1 = triPoints[theLocalEdge1[0]] * PTA1->m_coefs[0] + triPoints[theLocalEdge1[1]] * PTA1->m_coefs[1];

        Triangle topTri;
        if (directOriented) 
        {
            topTri = Triangle(communID, PTA1->m_idPoint, PTA0->m_idPoint);
            baseQuadriID = { PTA1->m_idPoint, nextID, m_triangleToSplit->m_triangle[(communLocalID + 2) % 3], PTA0->m_idPoint };
            quadPoints = { p1 , triPoints[(communLocalID + 1) % 3], triPoints[(communLocalID + 2) % 3], p0 };
        }
        else
        {
            topTri = Triangle(communID, PTA0->m_idPoint, PTA1->m_idPoint);
            baseQuadriID = { PTA0->m_idPoint, nextID, m_triangleToSplit->m_triangle[(communLocalID + 2) % 3], PTA1->m_idPoint };
            quadPoints = { p0 , triPoints[(communLocalID + 1) % 3], triPoints[(communLocalID + 2) % 3], p1 };
        }

        // compute diagonals
        auto diag0 = (quadPoints[0] - quadPoints[2]).norm2();
        auto diag1 = (quadPoints[1] - quadPoints[3]).norm2();

        Triangle bottomTri0, bottomTri1;
        if (diag0 < diag1)
        {
            bottomTri0 = Triangle(baseQuadriID[0], baseQuadriID[1], baseQuadriID[2]);
            bottomTri1 = Triangle(baseQuadriID[2], baseQuadriID[3], baseQuadriID[0]);
        }
        else
        {
            bottomTri0 = Triangle(baseQuadriID[0], baseQuadriID[1], baseQuadriID[3]);
            bottomTri1 = Triangle(baseQuadriID[1], baseQuadriID[2], baseQuadriID[3]);
        }

        auto TTA0 = new TriangleToAdd(1000000 * m_triangleToSplit->m_triangleId + 1, topTri, ancestors, coefs);
        auto TTA1 = new TriangleToAdd(1000000 * m_triangleToSplit->m_triangleId + 2, bottomTri0, ancestors, coefs);
        auto TTA2 = new TriangleToAdd(1000000 * m_triangleToSplit->m_triangleId + 2, bottomTri1, ancestors, coefs);

        m_trianglesToAdd.push_back(TTA0);
        m_trianglesToAdd.push_back(TTA1);
        m_trianglesToAdd.push_back(TTA2);

        return true;
    }

    EdgeID m_localEdgeId0;
    EdgeID m_localEdgeId1;
};


class TriangleSubdivider_2Node : public TriangleSubdivider
{
public:
    TriangleSubdivider_2Node(TriangleToSplit* _triangleToSplit) : TriangleSubdivider(_triangleToSplit) {}

    EdgeID m_edge1Id;
    SReal m_edge1Coef;
    sofa::type::Vec<3, SReal> m_baryCoords;
};


class TriangleSubdivider_3N : public TriangleSubdivider
{
public:
    TriangleSubdivider_3N(TriangleToSplit* _triangleToSplit) : TriangleSubdivider(_triangleToSplit) {}

    EdgeID m_edge1Id;
    SReal m_edge1Coef;
    EdgeID m_edge2Id;
    SReal m_edge2Coef;
    EdgeID m_edge3Id;
    SReal m_edge3Coef;
};




} //namespace sofa::component::topology::container::dynamic
