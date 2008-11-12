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
#ifndef SOFA_COMPONENT_TOPOLOGY_TRIANGLESETGEOMETRYALGORITHMS_INL
#define SOFA_COMPONENT_TOPOLOGY_TRIANGLESETGEOMETRYALGORITHMS_INL

#include <sofa/component/topology/TriangleSetGeometryAlgorithms.h>
#include <sofa/component/topology/CommonAlgorithms.h>

namespace sofa
{

namespace component
{

namespace topology
{
template< class DataTypes>
void TriangleSetGeometryAlgorithms< DataTypes >::computeTriangleAABB(const TriangleID i, Coord& minCoord, Coord& maxCoord) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    for(unsigned int i=0; i<3; ++i)
    {
        minCoord[i] = std::min(p[t[0]][i], std::min(p[t[1]][i], p[t[2]][i]));
        maxCoord[i] = std::max(p[t[0]][i], std::max(p[t[1]][i], p[t[2]][i]));
    }
}

template<class DataTypes>
typename DataTypes::Coord TriangleSetGeometryAlgorithms<DataTypes>::computeTriangleCenter(const TriangleID i) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    return (p[t[0]] + p[t[1]] + p[t[2]]) / (Real) 3.0;
}

template<class DataTypes>
void TriangleSetGeometryAlgorithms<DataTypes>::computeTriangleCircumcenterBaryCoefs(Vec<3,Real> &baryCoord, const TriangleID i) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());
    Real a2, b2, c2; // square legths of the 3 edges
    a2 = (p[t[1]]-p[t[0]]).norm2();
    b2 = (p[t[2]]-p[t[1]]).norm2();
    c2 = (p[t[0]]-p[t[2]]).norm2();

    // barycentric coordinates are defined as
    baryCoord = Vec<3,Real>(a2*(-a2+b2+c2), b2*(a2-b2+c2), c2*(a2+b2-c2));
}

template<class DataTypes>
typename DataTypes::Coord TriangleSetGeometryAlgorithms<DataTypes>::computeTriangleCircumcenter(const TriangleID i) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    Vec<3,Real> barycentricCoords;
    computeTriangleCircumcenterBaryCoefs(barycentricCoords, i);

    return (p[t[0]]*barycentricCoords[0] + p[t[1]]*barycentricCoords[1] + p[t[2]]*barycentricCoords[2]);
}

template< class DataTypes>
void TriangleSetGeometryAlgorithms< DataTypes >::getTriangleVertexCoordinates(const TriangleID i, Coord pnt[3]) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    for(unsigned int i=0; i<3; ++i)
    {
        pnt[i] = p[t[i]];
    }
}

template< class DataTypes>
void TriangleSetGeometryAlgorithms< DataTypes >::getRestTriangleVertexCoordinates(const TriangleID i, Coord pnt[3]) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX0());

    for(unsigned int i=0; i<3; ++i)
    {
        pnt[i] = p[t[i]];
    }
}

template< class DataTypes>
typename DataTypes::Real TriangleSetGeometryAlgorithms< DataTypes >::computeTriangleArea( const TriangleID i) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());
    Real area = (Real)(areaProduct(p[t[1]]-p[t[0]], p[t[2]]-p[t[0]]) * 0.5);
    return area;
}

template< class DataTypes>
typename DataTypes::Real TriangleSetGeometryAlgorithms< DataTypes >::computeRestTriangleArea( const TriangleID i) const
{
    const Triangle &t = this->m_topology->getTriangle(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX0());
    Real area = (Real) (areaProduct(p[t[1]]-p[t[0]],p[t[2]]-p[t[0]]) * 0.5);
    return area;
}

/// computes the edge length of all edges are store in the array interface
template<class DataTypes>
void TriangleSetGeometryAlgorithms<DataTypes>::computeTriangleArea( BasicArrayInterface<Real> &ai) const
{
    const sofa::helper::vector<Triangle> &ta = this->m_topology->getTriangles();
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    for (unsigned int i=0; i<ta.size(); ++i)
    {
        const Triangle &t=ta[i];
        ai[i]=(Real)(areaProduct(p[t[1]]-p[t[0]],p[t[2]]-p[t[0]]) * 0.5);
    }
}

// Computes the point defined by 2 indices of vertex and 1 barycentric coordinate
template<class DataTypes>
Vec<3,double> TriangleSetGeometryAlgorithms< DataTypes >::computeBaryEdgePoint(unsigned int p0, unsigned int p1, double coord_p) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    Vec<3,double> c0; c0 = vect_c[p0];
    Vec<3,double> c1; c1 = vect_c[p1];
    return c0*(1-coord_p) + c1*coord_p;
}

// Computes the opposite point to ind_p
template<class DataTypes>
Vec<3,double> TriangleSetGeometryAlgorithms< DataTypes >::getOppositePoint(unsigned int ind_p,
        const Edge& indices,
        double coord_p) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const typename DataTypes::Coord& c1 = vect_c[indices[0]];
    const typename DataTypes::Coord& c2 = vect_c[indices[1]];

    Vec<3,Real> p;

    if(ind_p == indices[0])
    {
        p[0]= (Real) c2[0];
        p[1]= (Real) c2[1];
        p[2]= (Real) c2[2];
    }
    else
    {
        if(ind_p == indices[1])
        {
            p[0]= (Real) c1[0];
            p[1]= (Real) c1[1];
            p[2]= (Real) c1[2];
        }
        else
        {
            p[0]= (Real) ((1.0-coord_p)*c1[0] + coord_p*c2[0]);
            p[1]= (Real) ((1.0-coord_p)*c1[1] + coord_p*c2[1]);
            p[2]= (Real) ((1.0-coord_p)*c1[2] + coord_p*c2[2]);
        }
    }

    return ((Vec<3,double>) p);
}

// Computes the normal vector of a triangle indexed by ind_t (not normed)
template<class DataTypes>
Vec<3,double> TriangleSetGeometryAlgorithms< DataTypes >::computeTriangleNormal(const TriangleID ind_t) const
{
    const Triangle &t = this->m_topology->getTriangle(ind_t);
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const typename DataTypes::Coord& c0 = vect_c[t[0]];
    const typename DataTypes::Coord& c1 = vect_c[t[1]];
    const typename DataTypes::Coord& c2 = vect_c[t[2]];

    Vec<3,Real> p0;
    p0[0] = (Real) (c0[0]);
    p0[1] = (Real) (c0[1]);
    p0[2] = (Real) (c0[2]);
    Vec<3,Real> p1;
    p1[0] = (Real) (c1[0]);
    p1[1] = (Real) (c1[1]);
    p1[2] = (Real) (c1[2]);
    Vec<3,Real> p2;
    p2[0] = (Real) (c2[0]);
    p2[1] = (Real) (c2[1]);
    p2[2] = (Real) (c2[2]);

    Vec<3,Real> normal_t=(p1-p0).cross( p2-p0);

    return ((Vec<3,double>) normal_t);
}

// barycentric coefficients of point p in triangle (a,b,c) indexed by ind_t
template<class DataTypes>
sofa::helper::vector< double > TriangleSetGeometryAlgorithms< DataTypes >::computeTriangleBarycoefs(
    const TriangleID ind_t,
    const Vec<3,double> &p) const
{
    const Triangle &t=this->m_topology->getTriangle(ind_t);
    return compute3PointsBarycoefs(p, t[0], t[1], t[2]);
}

// barycentric coefficients of point p in triangle whose vertices are indexed by (ind_p1,ind_p2,ind_p3)
template<class DataTypes>
sofa::helper::vector< double > TriangleSetGeometryAlgorithms< DataTypes >::compute3PointsBarycoefs(
    const Vec<3,double> &p,
    unsigned int ind_p1,
    unsigned int ind_p2,
    unsigned int ind_p3) const
{
    sofa::helper::vector< double > baryCoefs;

    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const typename DataTypes::Coord& c0 = vect_c[ind_p1];
    const typename DataTypes::Coord& c1 = vect_c[ind_p2];
    const typename DataTypes::Coord& c2 = vect_c[ind_p3];

    Vec<3,Real> a;
    a[0] = (Real) (c0[0]);
    a[1] = (Real) (c0[1]);
    a[2] = (Real) (c0[2]);
    Vec<3,Real> b;
    b[0] = (Real) (c1[0]);
    b[1] = (Real) (c1[1]);
    b[2] = (Real) (c1[2]);
    Vec<3,Real> c;
    c[0] = (Real) (c2[0]);
    c[1] = (Real) (c2[1]);
    c[2] = (Real) (c2[2]);

    Vec<3,double> M = (Vec<3,double>) (b-a).cross(c-a);
    double norm2_M = M*(M);

    double coef_a, coef_b, coef_c;

    if(norm2_M==0.0) // triangle (a,b,c) is flat
    {
        coef_a = (double) (1.0/3.0);
        coef_b = (double) (1.0/3.0);
        coef_c = (double) (1.0 - (coef_a + coef_b));
    }
    else
    {
        Vec<3,Real> N =  M/norm2_M;

        coef_a = N*((b-p).cross(c-p));
        coef_b = N*((c-p).cross(a-p));
        coef_c = (double) (1.0 - (coef_a + coef_b)); //N*((a-p).cross(b-p));
    }

    baryCoefs.push_back(coef_a);
    baryCoefs.push_back(coef_b);
    baryCoefs.push_back(coef_c);

    return baryCoefs;
}

// Find the two closest points from two triangles (each of the point belonging to one triangle)
template<class DataTypes>
void TriangleSetGeometryAlgorithms< DataTypes >::computeClosestIndexPair(const TriangleID ind_ta, const TriangleID ind_tb,
        unsigned int &ind1, unsigned int &ind2) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const Triangle &ta=this->m_topology->getTriangle(ind_ta);
    const Triangle &tb=this->m_topology->getTriangle(ind_tb);

    Real min_value=(Real) 0.0;
    bool is_init = false;

    for(unsigned int i=0; i<3; i++)
    {
        const typename DataTypes::Coord& ca=vect_c[ta[i]];
        Vec<3,Real> pa;
        pa[0] = (Real) (ca[0]);
        pa[1] = (Real) (ca[1]);
        pa[2] = (Real) (ca[2]);

        for(unsigned int j=0; j!=i && j<3; j++)
        {
            const typename DataTypes::Coord& cb=vect_c[tb[i]];
            Vec<3,Real> pb;
            pb[0] = (Real) (cb[0]);
            pb[1] = (Real) (cb[1]);
            pb[2] = (Real) (cb[2]);

            Real norm_v_normal = (pa-pb)*(pa-pb);
            if(!is_init)
            {
                min_value = norm_v_normal;
                ind1 = ta[i];
                ind2 = tb[j];
                is_init = true;
            }
            else
            {
                if(norm_v_normal<min_value)
                {
                    min_value = norm_v_normal;
                    ind1 = ta[i];
                    ind2 = tb[j];
                }
            }
        }
    }

    return;
}

// test if a point is included in the triangle indexed by ind_t
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::isPointInTriangle(const TriangleID ind_t,
        bool is_tested,
        const Vec<3,Real>& p,
        unsigned int &ind_t_test) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());
    const Triangle &t=this->m_topology->getTriangle(ind_t);

    const typename DataTypes::Coord& c0=vect_c[t[0]];
    const typename DataTypes::Coord& c1=vect_c[t[1]];
    const typename DataTypes::Coord& c2=vect_c[t[2]];

    Vec<3,Real> ptest = p;

    Vec<3,Real> p0;
    p0[0] = (Real) (c0[0]);
    p0[1] = (Real) (c0[1]);
    p0[2] = (Real) (c0[2]);
    Vec<3,Real> p1;
    p1[0] = (Real) (c1[0]);
    p1[1] = (Real) (c1[1]);
    p1[2] = (Real) (c1[2]);
    Vec<3,Real> p2;
    p2[0] = (Real) (c2[0]);
    p2[1] = (Real) (c2[1]);
    p2[2] = (Real) (c2[2]);

    Vec<3,Real> v_normal = (p2-p0).cross(p1-p0);

    Real norm_v_normal = v_normal*(v_normal);
    if(norm_v_normal != 0.0)
    {
        Vec<3,Real> n_01 = (p1-p0).cross(v_normal);
        Vec<3,Real> n_12 = (p2-p1).cross(v_normal);
        Vec<3,Real> n_20 = (p0-p2).cross(v_normal);

        double v_01 = (double) ((ptest-p0)*(n_01));
        double v_12 = (double) ((ptest-p1)*(n_12));
        double v_20 = (double) ((ptest-p2)*(n_20));

        bool is_inside = (v_01 > 0.0) && (v_12 > 0.0) && (v_20 > 0.0);

        if(is_tested && (!is_inside))
        {
            sofa::helper::vector< unsigned int > shell;
            unsigned int ind_edge = 0;

            if(v_01 < 0.0)
            {
                if(v_12 < 0.0) /// vertex 1
                {
                    shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleVertexShell(t[1]));
                }
                else
                {
                    if(v_20 < 0.0) /// vertex 0
                    {
                        shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleVertexShell(t[0]));

                    }
                    else // v_01 < 0.0
                    {
                        ind_edge=this->m_topology->getEdgeIndex(t[0],t[1]);
                        shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
                    }
                }
            }
            else
            {
                if(v_12 < 0.0)
                {
                    if(v_20 < 0.0) /// vertex 2
                    {
                        shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleVertexShell(t[2]));

                    }
                    else // v_12 < 0.0
                    {
                        ind_edge=this->m_topology->getEdgeIndex(t[1],t[2]);
                        shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
                    }
                }
                else // v_20 < 0.0
                {
                    ind_edge=this->m_topology->getEdgeIndex(t[2],t[0]);
                    shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
                }
            }

            unsigned int i =0;
            bool is_in_next_triangle=false;
            unsigned int ind_triangle=0;
            unsigned ind_t_false_init;
            unsigned int &ind_t_false = ind_t_false_init;

            if(shell.size()>1)
            {
                while(i < shell.size() && !is_in_next_triangle)
                {
                    ind_triangle=shell[i];

                    if(ind_triangle != ind_t)
                    {
                        is_in_next_triangle = isPointInTriangle(ind_triangle, false, p, ind_t_false);
                    }
                    i++;
                }

                if(is_in_next_triangle)
                {
                    ind_t_test=ind_triangle;
                    //std::cout << "correct to triangle indexed by " << ind_t_test << std::endl;
                }
                else // not found
                {
                    //std::cout << "not found !!! " << std::endl;
                    ind_t_test=ind_t;
                }
            }
            else
            {
                ind_t_test=ind_t;
            }
        }
        return is_inside;

    }
    else // triangle is flat
    {
        //std::cout << "INFO_print : triangle is flat" << std::endl;
        return false;
    }
}

// Tests how to triangularize a quad whose vertices are defined by (p_q1, p_q2, ind_q3, ind_q4) according to the Delaunay criterion
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::isQuadDeulaunayOriented(const Vec<3,double>& p_q1,
        const Vec<3,double>& p_q2,
        unsigned int ind_q3,
        unsigned int ind_q4) const
{
    sofa::helper::vector< double > baryCoefs;

    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const typename DataTypes::Coord& c3 = vect_c[ind_q3];
    const typename DataTypes::Coord& c4 = vect_c[ind_q4];

    Vec<3,double> p1 = p_q1;
    Vec<3,double> p2 = p_q2;
    Vec<3,double> p3; p3 = c3;
    Vec<3,double> p4; p4 = c4;
    return isQuadDeulaunayOriented(p1, p2, p3, p4);
}

/** \brief Tests how to triangularize a quad whose vertices are defined by (p1, p2, p3, p4) according to the Delaunay criterion
 *
 */
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::isQuadDeulaunayOriented(const Vec<3,double>& q1,
        const sofa::defaulttype::Vec<3,double>& q2,
        const sofa::defaulttype::Vec<3,double>& q3,
        const sofa::defaulttype::Vec<3,double>& q4) const
{
    Vec<3,double> G = (q1+q2+q3)/3.0;

    if((G-q2)*(G-q2) <= (G-q4)*(G-q4))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Test if a triangle indexed by ind_triangle (and incident to the vertex indexed by ind_p) is included or not in the plane defined by (ind_p, plane_vect)
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::isTriangleInPlane(const TriangleID ind_t,
        const unsigned int ind_p,
        const Vec<3,Real>&plane_vect) const
{
    const Triangle &t=this->m_topology->getTriangle(ind_t);

    // HYP : ind_p==t[0] or ind_p==t[1] or ind_p==t[2]

    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    unsigned int ind_1;
    unsigned int ind_2;

    if(ind_p==t[0])
    {
        ind_1=t[1];
        ind_2=t[2];
    }
    else
    {
        if(ind_p==t[1])
        {
            ind_1=t[2];
            ind_2=t[0];
        }
        else // ind_p==t[2]
        {
            ind_1=t[0];
            ind_2=t[1];
        }
    }

    const typename DataTypes::Coord& c0=vect_c[ind_p];
    const typename DataTypes::Coord& c1=vect_c[ind_1];
    const typename DataTypes::Coord& c2=vect_c[ind_2];

    Vec<3,Real> p0;
    p0[0] = (Real) (c0[0]);
    p0[1] = (Real) (c0[1]);
    p0[2] = (Real) (c0[2]);
    Vec<3,Real> p1;
    p1[0] = (Real) (c1[0]);
    p1[1] = (Real) (c1[1]);
    p1[2] = (Real) (c1[2]);
    Vec<3,Real> p2;
    p2[0] = (Real) (c2[0]);
    p2[1] = (Real) (c2[1]);
    p2[2] = (Real) (c2[2]);

    return((p1-p0)*( plane_vect)>=0.0 && (p1-p0)*( plane_vect)>=0.0);
}

// Prepares the duplication of a vertex
template<class DataTypes>
void TriangleSetGeometryAlgorithms< DataTypes >::prepareVertexDuplication(const unsigned int ind_p,
        const TriangleID ind_t_from,
        const TriangleID ind_t_to,
        const Edge& indices_from,
        const double &coord_from,
        const Edge& indices_to,
        const double &coord_to,
        sofa::helper::vector< unsigned int > &triangles_list_1,
        sofa::helper::vector< unsigned int > &triangles_list_2) const
{
    //HYP : if coord_from or coord_to == 0.0 or 1.0, ind_p is distinct from ind_from and from ind_to

    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const typename DataTypes::Coord& c_p = vect_c[ind_p];
    Vec<3,Real> point_p;
    point_p[0]= (Real) c_p[0];
    point_p[1]= (Real) c_p[1];
    point_p[2]= (Real) c_p[2];

    Vec<3,Real> point_from=(Vec<3,Real>) getOppositePoint(ind_p, indices_from, coord_from);
    Vec<3,Real> point_to=(Vec<3,Real>) getOppositePoint(ind_p, indices_to, coord_to);

    //Vec<3,Real> point_from=(Vec<3,Real>) computeBaryEdgePoint((sofa::helper::vector< unsigned int>&) indices_from, coord_from);
    //Vec<3,Real> point_to=(Vec<3,Real>) computeBaryEdgePoint((sofa::helper::vector< unsigned int>&) indices_to, coord_to);

    Vec<3,Real> vect_from = point_from - point_p;
    Vec<3,Real> vect_to = point_p - point_to;

    //std::cout << "INFO_print : vect_from = " << vect_from <<  std::endl;
    //std::cout << "INFO_print : vect_to = " << vect_to <<  std::endl;

    Vec<3,Real> normal_from;
    Vec<3,Real> normal_to;

    Vec<3,Real> plane_from;
    Vec<3,Real> plane_to;

    if((coord_from!=0.0) && (coord_from!=1.0))
    {
        normal_from=(Vec<3,Real>) computeTriangleNormal(ind_t_from);
        plane_from=vect_from.cross( normal_from); // inverse ??
    }
    else
    {
        // HYP : only 2 edges maximum are adjacent to the same triangle (otherwise : compute the one which minimizes the normed dotProduct and which gives the positive cross)

        unsigned int ind_edge;

        if(coord_from==0.0)
        {
            ind_edge=this->m_topology->getEdgeIndex(indices_from[0], ind_p);
        }
        else // coord_from==1.0
        {
            ind_edge=this->m_topology->getEdgeIndex(indices_from[1], ind_p);
        }

        if (this->m_topology->getNbEdges()>0)
        {
            sofa::helper::vector< unsigned int > shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
            unsigned int ind_triangle=shell[0];
            unsigned int i=0;
            bool is_in_next_triangle=false;

            if(shell.size()>1)
            {
                while(i < shell.size() || !is_in_next_triangle)
                {
                    if(shell[i] != ind_t_from)
                    {
                        ind_triangle=shell[i];
                        is_in_next_triangle=true;
                    }
                    i++;
                }
            }
            else
            {
                return;
            }

            if(is_in_next_triangle)
            {
                Vec<3,Real> normal_from_1=(Vec<3,Real>) computeTriangleNormal(ind_triangle);
                Vec<3,Real> normal_from_2=(Vec<3,Real>) computeTriangleNormal(ind_t_from);

                normal_from=(normal_from_1+normal_from_2)/2.0;
                plane_from=vect_from.cross( normal_from);
            }
            else
            {
                return;
            }
        }
    }

    if((coord_to!=0.0) && (coord_to!=1.0))
    {
        normal_to=(Vec<3,Real>) computeTriangleNormal(ind_t_to);

        plane_to=vect_to.cross( normal_to);
    }
    else
    {
        // HYP : only 2 edges maximum are adjacent to the same triangle (otherwise : compute the one which minimizes the normed dotProduct and which gives the positive cross)

        unsigned int ind_edge;

        if(coord_to==0.0)
        {
            ind_edge=this->m_topology->getEdgeIndex(indices_to[0], ind_p);
        }
        else // coord_to==1.0
        {
            ind_edge=this->m_topology->getEdgeIndex(indices_to[1], ind_p);
        }

        if (this->m_topology->getNbEdges()>0)
        {
            sofa::helper::vector< unsigned int > shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
            unsigned int ind_triangle=shell[0];
            unsigned int i=0;
            bool is_in_next_triangle=false;

            if(shell.size()>1)
            {
                while(i < shell.size() || !is_in_next_triangle)
                {
                    if(shell[i] != ind_t_to)
                    {
                        ind_triangle=shell[i];
                        is_in_next_triangle=true;
                    }
                    i++;
                }
            }
            else
            {
                return;
            }

            if(is_in_next_triangle)
            {
                Vec<3,Real> normal_to_1=(Vec<3,Real>) computeTriangleNormal(ind_triangle);
                Vec<3,Real> normal_to_2=(Vec<3,Real>) computeTriangleNormal(ind_t_to);

                normal_to=(normal_to_1+normal_to_2)/2.0;
                plane_to=vect_to.cross( normal_to);
            }
            else
            {
                return;
            }
        }
    }

    if (this->m_topology->getNbPoints()>0)
    {
        sofa::helper::vector< unsigned int > shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleVertexShell(ind_p));
        unsigned int ind_triangle=shell[0];
        unsigned int i=0;

        bool is_in_plane_from;
        bool is_in_plane_to;

        if(shell.size()>1)
        {
            Vec<3,Real> normal_test = plane_from.cross( plane_to);
            Real value_test =   normal_test*(normal_from+normal_to);

            if(value_test<=0.0)
            {
                //std::cout << "INFO_print : CONVEXE, value_test = " << value_test <<  std::endl;
                //std::cout << "INFO_print : shell.size() = " << shell.size() << ", ind_t_from = " << ind_t_from << ", ind_t_to = " << ind_t_to <<  std::endl;

                while(i < shell.size())
                {
                    ind_triangle=shell[i];

                    is_in_plane_from=isTriangleInPlane(ind_triangle,ind_p, (const Vec<3,double>&) plane_from);
                    is_in_plane_to=isTriangleInPlane(ind_triangle,ind_p, (const Vec<3,double>&) plane_to);

                    if((ind_triangle != ind_t_from) && (ind_triangle != ind_t_to))
                    {
                        if(is_in_plane_from || is_in_plane_to)
                        {
                            triangles_list_1.push_back(ind_triangle);
                        }
                        else
                        {
                            triangles_list_2.push_back(ind_triangle);
                        }
                    }
                    i++;
                }
            }
            else // value_test>0.0
            {
                //std::cout << "INFO_print : CONCAVE, value_test = " << value_test <<  std::endl;
                //std::cout << "INFO_print : shell.size() = " << shell.size() << ", ind_t_from = " << ind_t_from << ", ind_t_to = " << ind_t_to <<  std::endl;

                while(i < shell.size())
                {
                    ind_triangle=shell[i];

                    is_in_plane_from=isTriangleInPlane(ind_triangle,ind_p, (const Vec<3,double>&) plane_from);
                    is_in_plane_to=isTriangleInPlane(ind_triangle,ind_p, (const Vec<3,double>&) plane_to);

                    if((ind_triangle != ind_t_from) && (ind_triangle != ind_t_to))
                    {
                        if(is_in_plane_from && is_in_plane_to)
                        {
                            triangles_list_1.push_back(ind_triangle);
                        }
                        else
                        {
                            triangles_list_2.push_back(ind_triangle);
                        }
                    }
                    i++;
                }
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

// Computes the intersection of the segment from point a to point b and the triangle indexed by t
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::computeSegmentTriangleIntersection(bool is_entered,
        const Vec<3,double>& a,
        const Vec<3,double>& b,
        const TriangleID ind_t,
        sofa::helper::vector<unsigned int> &indices,
        double &baryCoef, double& coord_kmin) const
{
    // HYP : point a is in triangle indexed by t
    // is_entered == true => indices.size() == 2

    unsigned int ind_first=0;
    unsigned int ind_second=0;

    if(indices.size()>1)
    {
        ind_first=indices[0];
        ind_second=indices[1];
    }

    indices.clear();

    bool is_validated = false;
    bool is_intersected = false;

    const Triangle &t=this->m_topology->getTriangle(ind_t);
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    bool is_full_01=(is_entered && ((t[0] == ind_first && t[1] == ind_second) || (t[1] == ind_first && t[0] == ind_second)));
    bool is_full_12=(is_entered && ((t[1] == ind_first && t[2] == ind_second) || (t[2] == ind_first && t[1] == ind_second)));
    bool is_full_20=(is_entered && ((t[2] == ind_first && t[0] == ind_second) || (t[0] == ind_first && t[2] == ind_second)));

    const typename DataTypes::Coord& c0=vect_c[t[0]];
    const typename DataTypes::Coord& c1=vect_c[t[1]];
    const typename DataTypes::Coord& c2=vect_c[t[2]];

    Vec<3,Real> p0;
    p0[0] = (Real) (c0[0]);
    p0[1] = (Real) (c0[1]);
    p0[2] = (Real) (c0[2]);
    Vec<3,Real> p1;
    p1[0] = (Real) (c1[0]);
    p1[1] = (Real) (c1[1]);
    p1[2] = (Real) (c1[2]);
    Vec<3,Real> p2;
    p2[0] = (Real) (c2[0]);
    p2[1] = (Real) (c2[1]);
    p2[2] = (Real) (c2[2]);

    Vec<3,Real> pa;
    pa[0] = (Real) (a[0]);
    pa[1] = (Real) (a[1]);
    pa[2] = (Real) (a[2]);
    Vec<3,Real> pb;
    pb[0] = (Real) (b[0]);
    pb[1] = (Real) (b[1]);
    pb[2] = (Real) (b[2]);

    Vec<3,Real> v_normal = (p2-p0).cross(p1-p0);
    //Vec<3,Real> v_normal = (Vec<3,Real>) computeTriangleNormal(ind_t);

    Real norm_v_normal = v_normal.norm(); // WARN : square root COST

    if(norm_v_normal != 0.0)
    {
        v_normal/=norm_v_normal;

        Vec<3,Real> v_ab = pb-pa;
        Vec<3,Real> v_ab_proj = v_ab - v_normal * dot(v_ab,v_normal); // projection
        Vec<3,Real> pb_proj = v_ab_proj + pa;

        Vec<3,Real> v_01 = p1-p0;
        Vec<3,Real> v_12 = p2-p1;
        Vec<3,Real> v_20 = p0-p2;

        Vec<3,Real> n_proj =v_ab_proj.cross(v_normal);
        Vec<3,Real> n_01 = v_01.cross(v_normal);
        Vec<3,Real> n_12 = v_12.cross(v_normal);
        Vec<3,Real> n_20 = v_20.cross(v_normal);

        Real norm2_v_ab_proj = v_ab_proj*(v_ab_proj);

        if(norm2_v_ab_proj != 0.0) // pb_proj != pa
        {
            double coord_t=0.0;
            double coord_k=0.0;

            double is_initialized=false;
            coord_kmin=0.0;

            double coord_test1;
            double coord_test2;

            double s_t;
            double s_k;

            if(!is_full_01)
            {
                /// Test of edge (p0,p1) :

                s_t = (p0-p1)*n_proj;
                s_k = (pa-pb_proj)*n_01;

                // s_t == 0.0 iff s_k == 0.0

                if(s_t==0.0) // (pa,pb_proj) and (p0,p1) are parallel
                {
                    if((p0-pa)*(n_proj)==0.0) // (pa,pb_proj) and (p0,p1) are on the same line
                    {
                        coord_test1 = (pa-p0)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa
                        coord_test2 = (pa-p1)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa

                        if(coord_test1>=0)
                        {
                            coord_k=coord_test1;
                            coord_t=0.0;
                        }
                        else
                        {
                            coord_k=coord_test2;
                            coord_t=1.0;
                        }

                        is_intersected = (coord_k > 0.0 && (coord_t >= 0.0 && coord_t <= 1.0));

                    }
                    else  // (pa,pb_proj) and (p0,p1) are parallel and disjoint
                    {
                        is_intersected=false;
                    }
                }
                else // s_t != 0.0 and s_k != 0.0
                {
                    coord_k=double((pa-p0)*(n_01))*1.0/double(s_k);
                    coord_t=double((p0-pa)*(n_proj))*1.0/double(s_t);

                    is_intersected = ((coord_k > 0.0) && (coord_t >= 0.0 && coord_t <= 1.0));
                }

                if(is_intersected)
                {
                    if((!is_initialized) || (coord_k > coord_kmin))
                    {
                        indices.clear();
                        indices.push_back(t[0]);
                        indices.push_back(t[1]);
                        baryCoef=coord_t;
                        coord_kmin=coord_k;
                    }

                    is_initialized=true;
                }

                is_validated = is_validated || is_initialized;
            }

            if(!is_full_12)
            {
                /// Test of edge (p1,p2) :

                s_t = (p1-p2)*(n_proj);
                s_k = (pa-pb_proj)*(n_12);

                // s_t == 0.0 iff s_k == 0.0

                if(s_t==0.0) // (pa,pb_proj) and (p1,p2) are parallel
                {
                    if((p1-pa)*(n_proj)==0.0) // (pa,pb_proj) and (p1,p2) are on the same line
                    {
                        coord_test1 = (pa-p1)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa
                        coord_test2 = (pa-p2)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa

                        if(coord_test1>=0)
                        {
                            coord_k=coord_test1;
                            coord_t=0.0;
                        }
                        else
                        {
                            coord_k=coord_test2;
                            coord_t=1.0;
                        }

                        is_intersected = (coord_k > 0.0 && (coord_t >= 0.0 && coord_t <= 1.0));
                    }
                    else // (pa,pb_proj) and (p1,p2) are parallel and disjoint
                    {
                        is_intersected=false;
                    }

                }
                else   // s_t != 0.0 and s_k != 0.0
                {

                    coord_k=double((pa-p1)*(n_12))*1.0/double(s_k);
                    coord_t=double((p1-pa)*(n_proj))*1.0/double(s_t);

                    is_intersected = ((coord_k > 0.0) && (coord_t >= 0.0 && coord_t <= 1.0));
                }

                if(is_intersected)
                {
                    if((!is_initialized) || (coord_k > coord_kmin))
                    {
                        indices.clear();
                        indices.push_back(t[1]);
                        indices.push_back(t[2]);
                        baryCoef=coord_t;
                        coord_kmin=coord_k;
                    }

                    is_initialized=true;
                }

                is_validated = is_validated || is_initialized;
            }

            if(!is_full_20)
            {
                /// Test of edge (p2,p0) :

                s_t = (p2-p0)*(n_proj);
                s_k = (pa-pb_proj)*(n_20);

                // s_t == 0.0 iff s_k == 0.0

                if(s_t==0.0) // (pa,pb_proj) and (p2,p0) are parallel
                {
                    if((p2-pa)*(n_proj)==0.0) // (pa,pb_proj) and (p2,p0) are on the same line
                    {
                        coord_test1 = (pa-p2)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa
                        coord_test2 = (pa-p0)*(pa-pb_proj)/norm2_v_ab_proj; // HYP : pb_proj != pa

                        if(coord_test1>=0)
                        {
                            coord_k=coord_test1;
                            coord_t=0.0;
                        }
                        else
                        {
                            coord_k=coord_test2;
                            coord_t=1.0;
                        }

                        is_intersected = (coord_k > 0.0 && (coord_t >= 0.0 && coord_t <= 1.0));

                    }
                    else // (pa,pb_proj) and (p2,p0) are parallel and disjoint
                    {
                        is_intersected = false;
                    }
                }
                else // s_t != 0.0 and s_k != 0.0
                {
                    coord_k=double((pa-p2)*(n_20))*1.0/double(s_k);
                    coord_t=double((p2-pa)*(n_proj))*1.0/double(s_t);

                    is_intersected = ((coord_k > 0.0) && (coord_t >= 0.0 && coord_t <= 1.0));
                }

                if(is_intersected)
                {
                    if((!is_initialized) || (coord_k > coord_kmin))
                    {
                        indices.clear();
                        indices.push_back(t[2]);
                        indices.push_back(t[0]);
                        baryCoef=coord_t;
                        coord_kmin=coord_k;
                    }

                    is_initialized = true;
                }

                is_validated = is_validated || is_initialized;
            }
        }
        else
        {
            is_validated = false; // points a and b are projected to the same point on triangle t
        }
    }
    else
    {
        is_validated = false; // triangle t is flat
    }

    return is_validated;
}


// Computes the list of points (edge,coord) intersected by the segment from point a to point b
// and the triangular mesh
template<class DataTypes>
bool TriangleSetGeometryAlgorithms< DataTypes >::computeIntersectedPointsList(const Vec<3,double>& a,
        const Vec<3,double>& b,
        const unsigned int ind_ta,
        unsigned int& ind_tb,
        sofa::helper::vector< unsigned int > &triangles_list,
        sofa::helper::vector<unsigned int> &edges_list,
        sofa::helper::vector< double >& coords_list, bool& is_on_boundary) const
{
    bool is_validated=true;
    bool is_intersected=true;

    Vec<3,double> c_t_test = a;

    is_on_boundary = false;

    sofa::helper::vector<unsigned int> indices;

    double coord_t=0.0;
    double coord_k=0.0;
    double coord_k_test=0.0;

    Vec<3,double> p_current=a;
    unsigned int ind_t_current=ind_ta;

    unsigned int ind_edge;
    unsigned int ind_index;
    unsigned int ind_triangle = ind_ta;

    double dist_min=0.0;

    is_intersected=computeSegmentTriangleIntersection(false, p_current, b, (const unsigned int) ind_t_current, indices, coord_t, coord_k);

    coord_k_test=coord_k;
    dist_min=(b-a)*(b-a);

    while((coord_k_test<1.0 && is_validated) && is_intersected)
    {
        ind_edge=this->m_topology->getEdgeIndex(indices[0],indices[1]);
        edges_list.push_back(ind_edge);
        triangles_list.push_back(ind_t_current);
        if (this->m_topology->getEdge(ind_edge)[0] == indices[0])
            coords_list.push_back(coord_t);
        else
            coords_list.push_back(1.0-coord_t);

        const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

        Vec<3,double> c_t_current; // WARNING : conversion from 'double' to 'float', possible loss of data ! // typename DataTypes::Coord
        c_t_current[0]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][0]))+coord_t*((double) (vect_c[indices[1]][0])));
        c_t_current[1]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][1]))+coord_t*((double) (vect_c[indices[1]][1])));
        c_t_current[2]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][2]))+coord_t*((double) (vect_c[indices[1]][2])));

        p_current=c_t_current;

        Vec<3,Real> p_t_aux;
        p_t_aux[0] = (Real) (c_t_current[0]);
        p_t_aux[1] = (Real) (c_t_current[1]);
        p_t_aux[2] = (Real) (c_t_current[2]);

        if(coord_t==0.0 || coord_t==1.0) // current point indexed by ind_t_current is on a vertex
        {
            //std::cout << "INFO_print : INPUT ON A VERTEX !!!" <<  std::endl;

            if(coord_t==0.0)
            {
                ind_index=indices[0];
            }
            else // coord_t==1.0
            {
                ind_index=indices[1];
            }

            if (this->m_topology->getNbPoints() >0)
            {
                sofa::helper::vector< unsigned int > shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleVertexShell(ind_index));
                ind_triangle=shell[0];
                unsigned int i=0;
                //bool is_in_next_triangle=false;
                bool is_test_init=false;

                unsigned int ind_from = ind_t_current;

                if(shell.size()>1) // at leat one neighbor triangle which is not indexed by ind_t_current
                {
                    is_on_boundary=false;

                    while(i < shell.size())
                    {
                        if(shell[i] != ind_from)
                        {
                            ind_triangle=shell[i];

                            const Triangle &t=this->m_topology->getTriangle(ind_triangle);

                            const typename DataTypes::Coord& c0=vect_c[t[0]];
                            const typename DataTypes::Coord& c1=vect_c[t[1]];
                            const typename DataTypes::Coord& c2=vect_c[t[2]];

                            Vec<3,Real> p0_aux;
                            p0_aux[0] = (Real) (c0[0]);
                            p0_aux[1] = (Real) (c0[1]);
                            p0_aux[2] = (Real) (c0[2]);
                            Vec<3,Real> p1_aux;
                            p1_aux[0] = (Real) (c1[0]);
                            p1_aux[1] = (Real) (c1[1]);
                            p1_aux[2] = (Real) (c1[2]);
                            Vec<3,Real> p2_aux;
                            p2_aux[0] = (Real) (c2[0]);
                            p2_aux[1] = (Real) (c2[1]);
                            p2_aux[2] = (Real) (c2[2]);

                            is_intersected=computeSegmentTriangleIntersection(true, p_current, b, ind_triangle, indices, coord_t, coord_k);

                            if(is_intersected)
                            {
                                Vec<3,double> c_t_test; // WARNING : conversion from 'double' to 'float', possible loss of data ! // typename DataTypes::Coord
                                c_t_test[0]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][0]))+coord_t*((double) (vect_c[indices[1]][0])));
                                c_t_test[1]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][1]))+coord_t*((double) (vect_c[indices[1]][1])));
                                c_t_test[2]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][2]))+coord_t*((double) (vect_c[indices[1]][2])));

                                double dist_test=(b-c_t_test)*(b-c_t_test);

                                if(is_test_init)
                                {
                                    if(dist_test<dist_min && coord_k<=1) //dist_test<dist_min
                                    {
                                        coord_k_test=coord_k;
                                        dist_min=dist_test;
                                        ind_t_current=ind_triangle;
                                    }
                                }
                                else
                                {
                                    is_test_init=true;
                                    coord_k_test=coord_k;
                                    dist_min=dist_test;
                                    ind_t_current=ind_triangle;
                                }
                            }
                        }

                        i=i+1;
                    }

                    is_intersected=is_test_init;

                }
                else
                {
                    is_on_boundary=true;
                    is_validated=false;
                }
            }
            else
            {
                is_validated=false;
            }
        }
        else // current point indexed by ind_t_current is on an edge, but not on a vertex
        {
            ind_edge=this->m_topology->getEdgeIndex(indices[0],indices[1]);

            if (this->m_topology->getNbEdges()>0)
            {
                sofa::helper::vector< unsigned int > shell =(sofa::helper::vector< unsigned int >) (this->m_topology->getTriangleEdgeShell(ind_edge));
                ind_triangle=shell[0];
                unsigned int i=0;

                bool is_test_init=false;

                unsigned int ind_from = ind_t_current;

                if(shell.size()>1) // at leat one neighbor triangle which is not indexed by ind_t_current
                {
                    is_on_boundary=false;

                    while(i < shell.size())
                    {
                        if(shell[i] != ind_from)
                        {
                            ind_triangle=shell[i];

                            const Triangle &t=this->m_topology->getTriangle(ind_triangle);

                            const typename DataTypes::Coord& c0=vect_c[t[0]];
                            const typename DataTypes::Coord& c1=vect_c[t[1]];
                            const typename DataTypes::Coord& c2=vect_c[t[2]];

                            Vec<3,Real> p0_aux;
                            p0_aux[0] = (Real) (c0[0]);
                            p0_aux[1] = (Real) (c0[1]);
                            p0_aux[2] = (Real) (c0[2]);
                            Vec<3,Real> p1_aux;
                            p1_aux[0] = (Real) (c1[0]);
                            p1_aux[1] = (Real) (c1[1]);
                            p1_aux[2] = (Real) (c1[2]);
                            Vec<3,Real> p2_aux;
                            p2_aux[0] = (Real) (c2[0]);
                            p2_aux[1] = (Real) (c2[1]);
                            p2_aux[2] = (Real) (c2[2]);

                            is_intersected=computeSegmentTriangleIntersection(true, p_current, b, ind_triangle, indices, coord_t, coord_k);

                            if(is_intersected)
                            {
                                //Vec<3,double> c_t_test; // WARNING : conversion from 'double' to 'float', possible loss of data ! // typename DataTypes::Coord
                                c_t_test[0]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][0]))+coord_t*((double) (vect_c[indices[1]][0])));
                                c_t_test[1]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][1]))+coord_t*((double) (vect_c[indices[1]][1])));
                                c_t_test[2]=(double) ((1.0-coord_t)*((double) (vect_c[indices[0]][2]))+coord_t*((double) (vect_c[indices[1]][2])));

                                double dist_test=(b-c_t_test)*(b-c_t_test);

                                if(is_test_init)
                                {
                                    if(dist_test<dist_min && coord_k<=1) //dist_test<dist_min
                                    {
                                        coord_k_test=coord_k;
                                        dist_min=dist_test;
                                        ind_t_current=ind_triangle;
                                    }
                                }
                                else
                                {
                                    is_test_init=true;
                                    coord_k_test=coord_k;
                                    dist_min=dist_test;
                                    ind_t_current=ind_triangle;
                                }
                            }
                        }

                        i=i+1;
                    }

                    is_intersected=is_test_init;
                }
                else
                {
                    is_on_boundary=true;
                    is_validated=false;
                }
            }
            else
            {
                is_validated=false;
            }
        }
    }

    if (ind_tb == (unsigned int)-1)
        ind_tb = ind_triangle;

    bool is_reached = (ind_tb==ind_triangle && coord_k_test>=1.0);

    if(is_reached)
    {
        std::cout << "INFO_print - TriangleSetTopology.inl : Cut is reached" << std::endl;
    }

    if(is_on_boundary)
    {
        std::cout << "INFO_print - TriangleSetTopology.inl : Cut meets a mesh boundary" << std::endl;
    }

    if(!is_reached && !is_on_boundary)
    {
        std::cout << "INFO_print - TriangleSetTopology.inl : Cut is not reached" << std::endl;
        ind_tb=ind_triangle;
    }

    return (is_reached && is_validated && is_intersected); // b is in triangle indexed by ind_t_current
}

/// Get the triangle in a given direction from a point.
template <typename DataTypes>
int TriangleSetGeometryAlgorithms<DataTypes>::getTriangleInDirection(PointID p, const sofa::defaulttype::Vec<3,double>& dir) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());
    const sofa::helper::vector<TriangleID> &shell=this->m_topology->getTriangleVertexShell(p);
    Vec<3,Real> dtest = dir;
    for (unsigned int i=0; i<shell.size(); ++i)
    {
        unsigned int ind_t = shell[i];
        const Triangle &t=this->m_topology->getTriangle(ind_t);

        const typename DataTypes::Coord& c0=vect_c[t[0]];
        const typename DataTypes::Coord& c1=vect_c[t[1]];
        const typename DataTypes::Coord& c2=vect_c[t[2]];

        Vec<3,Real> p0;
        p0[0] = (Real) (c0[0]);
        p0[1] = (Real) (c0[1]);
        p0[2] = (Real) (c0[2]);
        Vec<3,Real> p1;
        p1[0] = (Real) (c1[0]);
        p1[1] = (Real) (c1[1]);
        p1[2] = (Real) (c1[2]);
        Vec<3,Real> p2;
        p2[0] = (Real) (c2[0]);
        p2[1] = (Real) (c2[1]);
        p2[2] = (Real) (c2[2]);

        Vec<3,Real> e1, e2;
        if (t[0] == p) { e1 = p1-p0; e2 = p2-p0; }
        else if (t[1] == p) { e1 = p2-p1; e2 = p0-p1; }
        else { e1 = p0-p2; e2 = p1-p2; }

        Vec<3,Real> v_normal = (e2).cross(e1);

        if(v_normal.norm2() > 1e-20)
        {
            Vec<3,Real> n_01 = e1.cross(v_normal);
            Vec<3,Real> n_02 = e2.cross(v_normal);

            double v_01 = (double) ((dtest)*(n_01));
            double v_02 = (double) ((dtest)*(n_02));

            bool is_inside = (v_01 >= 0.0) && (v_02 < 0.0);
            if (is_inside) return ind_t;
        }
    }
    return (TriangleID)-1;
}

/// Write the current mesh into a msh file
template <typename DataTypes>
void TriangleSetGeometryAlgorithms<DataTypes>::writeMSHfile(const char *filename) const
{
    std::ofstream myfile;
    myfile.open (filename);

    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const unsigned int numVertices = vect_c.size();

    myfile << "$NOD\n";
    myfile << numVertices <<"\n";

    for (unsigned int i=0; i<numVertices; ++i)
    {
        double x = (double) vect_c[i][0];
        double y = (double) vect_c[i][1];
        double z = (double) vect_c[i][2];

        myfile << i+1 << " " << x << " " << y << " " << z <<"\n";
    }

    myfile << "$ENDNOD\n";
    myfile << "$ELM\n";

    const sofa::helper::vector<Triangle> &ta=this->m_topology->getTriangles();

    myfile << ta.size() <<"\n";

    for (unsigned int i=0; i<ta.size(); ++i)
    {
        myfile << i+1 << " 2 6 6 3 " << ta[i][0]+1 << " " << ta[i][1]+1 << " " << ta[i][2]+1 <<"\n";
    }

    myfile << "$ENDELM\n";

    myfile.close();
}

/// Test for REAL if a point p is in a triangle indexed by (a,b,c)

template<class Real>
bool is_point_in_triangle(const Vec<3,Real>& p, const Vec<3,Real>& a, const Vec<3,Real>& b, const Vec<3,Real>& c)
{
    Vec<3,Real> ptest = p;

    Vec<3,Real> p0 = a;
    Vec<3,Real> p1 = b;
    Vec<3,Real> p2 = c;

    Vec<3,Real> v_normal = (p2-p0).cross(p1-p0);

    Real norm_v_normal = v_normal*(v_normal);
    if(norm_v_normal != 0.0)
    {
        //v_normal/=norm_v_normal;

        if((ptest-p0)*(v_normal)==0.0) // p is in the plane defined by the triangle (p0,p1,p2)
        {

            Vec<3,Real> n_01 = (p1-p0).cross(v_normal);
            Vec<3,Real> n_12 = (p2-p1).cross(v_normal);
            Vec<3,Real> n_20 = (p0-p2).cross(v_normal);

            return (((ptest-p0)*(n_01) >= 0.0) && ((ptest-p1)*(n_12) >= 0.0) && ((ptest-p2)*(n_20) >= 0.0));

        }
        else // p is not in the plane defined by the triangle (p0,p1,p2)
        {
            //std::cout << "INFO_print : p is not in the plane defined by the triangle (p0,p1,p2)" << std::endl;
            return false;
        }

    }
    else // triangle is flat
    {
        //std::cout << "INFO_print : triangle is flat" << std::endl;
        return false;
    }
}

/// Test if a point p is in the right halfplane

template<class Real>
bool is_point_in_halfplane(const Vec<3,Real>& p, unsigned int e0, unsigned int e1,
        const Vec<3,Real>& a, const Vec<3,Real>& b, const Vec<3,Real>& c,
        unsigned int ind_p0, unsigned int ind_p1, unsigned int ind_p2)
{
    Vec<3,Real> ptest = p;

    Vec<3,Real> p0 = a;
    Vec<3,Real> p1 = b;
    Vec<3,Real> p2 = c;

    Vec<3,Real> v_normal = (p2-p0).cross(p1-p0);

    Real norm_v_normal = (v_normal)*(v_normal);

    if(norm_v_normal != 0.0)
    {
        //v_normal/=norm_v_normal;

        if(ind_p0==e0 || ind_p0==e1)
        {
            Vec<3,Real> n_12 = (p2-p1).cross(v_normal);
            return ((ptest-p1)*(n_12) >= 0.0);
        }
        else
        {
            if(ind_p1==e0 || ind_p1==e1)
            {
                Vec<3,Real> n_20 = (p0-p2).cross(v_normal);
                return ((ptest-p2)*(n_20) >= 0.0);
            }
            else
            {
                if(ind_p2==e0 || ind_p2==e1)
                {
                    Vec<3,Real> n_01 = (p1-p0).cross(v_normal);
                    return ((ptest-p0)*(n_01) >= 0.0);
                }
                else
                {
                    return false; // not expected
                }
            }
        }
    }
    else // triangle is flat
    {
        //std::cout << "INFO_print : triangle is flat" << std::endl;
        return false;
    }
}

} // namespace topology

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENTS_TriangleSetTOPOLOGY_INL
