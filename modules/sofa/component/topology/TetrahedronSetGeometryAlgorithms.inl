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
#ifndef SOFA_COMPONENT_TOPOLOGY_TETRAHEDRONSETGEOMETRYALGORITHMS_INL
#define SOFA_COMPONENT_TOPOLOGY_TETRAHEDRONSETGEOMETRYALGORITHMS_INL

#include <sofa/component/topology/TetrahedronSetGeometryAlgorithms.h>
#include <sofa/component/topology/CommonAlgorithms.h>

namespace sofa
{

namespace component
{

namespace topology
{
using namespace sofa::defaulttype;

template< class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::computeTetrahedronAABB(const TetraID i, Coord& minCoord, Coord& maxCoord) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    for(unsigned int i=0; i<3; ++i)
    {
        minCoord[i] = std::min(std::min(p[t[0]][i], p[t[3]][i]), std::min(p[t[1]][i], p[t[2]][i]));
        maxCoord[i] = std::max(std::max(p[t[0]][i], p[t[3]][i]), std::max(p[t[1]][i], p[t[2]][i]));
    }
}

template<class DataTypes>
typename DataTypes::Coord TetrahedronSetGeometryAlgorithms<DataTypes>::computeTetrahedronCenter(const TetraID i) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    return (p[t[0]] + p[t[1]] + p[t[2]] + p[t[3]]) * (Real) 0.25;
}

template<class DataTypes>
typename DataTypes::Coord TetrahedronSetGeometryAlgorithms<DataTypes>::computeTetrahedronCircumcenter(const TetraID i) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    Coord center = p[t[0]];
    Coord t1 = p[t[1]] - p[t[0]];
    Coord t2 = p[t[2]] - p[t[0]];
    Coord t3 = p[t[3]] - p[t[0]];
    Vec<3,Real> a(t1[0], t1[1], t1[2]);
    Vec<3,Real> b(t2[0], t2[1], t2[2]);
    Vec<3,Real> c(t3[0], t3[1], t3[2]);

//		using namespace sofa::defaulttype;
    Vec<3,Real> d = (cross(b, c) * a.norm2() + cross(c, a) * b.norm2() + cross(a, b) * c.norm2()) / (12* computeTetrahedronVolume(i));

    center[0] += d[0];
    center[1] += d[1];
    center[2] += d[2];

    return center;
}

template< class DataTypes>
bool TetrahedronSetGeometryAlgorithms< DataTypes >::isPointInTetrahedron(const TetraID ind_t, const Vec<3,Real>& pTest) const
{
    const double ZERO = 1e-12;

    const Tetrahedron t = this->m_topology->getTetrahedron(ind_t);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    const Vec<3,Real> t0(p[t[0]][0], p[t[0]][1], p[t[0]][2]);
    const Vec<3,Real> t1(p[t[1]][0], p[t[1]][1], p[t[1]][2]);
    const Vec<3,Real> t2(p[t[2]][0], p[t[2]][1], p[t[2]][2]);
    const Vec<3,Real> t3(p[t[3]][0], p[t[3]][1], p[t[3]][2]);

    double v0 = tripleProduct(t1-pTest, t2-pTest, t3-pTest);
    double v1 = tripleProduct(pTest-t0, t2-t0, t3-t0);
    double v2 = tripleProduct(t1-t0, pTest-t0, t3-t0);
    double v3 = tripleProduct(t1-t0, t2-t0, pTest-t0);

    return (v0 > -ZERO) && (v1 > -ZERO) && (v2 > -ZERO) && (v3 > -ZERO);
}

template< class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::getTetrahedronVertexCoordinates(const TetraID i, Coord pnt[4]) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());

    for(unsigned int i=0; i<4; ++i)
    {
        pnt[i] = p[t[i]];
    }
}

template< class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::getRestTetrahedronVertexCoordinates(const TetraID i, Coord pnt[4]) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX0());

    for(unsigned int i=0; i<4; ++i)
    {
        pnt[i] = p[t[i]];
    }
}

template< class DataTypes>
typename DataTypes::Real TetrahedronSetGeometryAlgorithms< DataTypes >::computeTetrahedronVolume( const TetraID i) const
{
    const Tetrahedron t = this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX());
    Real volume = (Real)(tripleProduct(p[t[1]]-p[t[0]],p[t[2]]-p[t[0]],p[t[3]]-p[t[0]])/6.0);
    return volume;
}

template< class DataTypes>
typename DataTypes::Real TetrahedronSetGeometryAlgorithms< DataTypes >::computeRestTetrahedronVolume( const TetraID i) const
{
    const Tetrahedron t=this->m_topology->getTetrahedron(i);
    const typename DataTypes::VecCoord& p = *(this->object->getX0());
    Real volume = (Real)(tripleProduct(p[t[1]]-p[t[0]],p[t[2]]-p[t[0]],p[t[3]]-p[t[0]])/6.0);
    return volume;
}

/// computes the edge length of all edges are store in the array interface
template<class DataTypes>
void TetrahedronSetGeometryAlgorithms<DataTypes>::computeTetrahedronVolume( BasicArrayInterface<Real> &ai) const
{
    const sofa::helper::vector<Tetrahedron> &ta = this->m_topology->getTetrahedra();
    const typename DataTypes::VecCoord& p = *(this->object->getX());
    for (unsigned int i=0; i<ta.size(); ++i)
    {
        const Tetrahedron &t = ta[i];
        ai[i] = (Real)(tripleProduct(p[t[1]]-p[t[0]],p[t[2]]-p[t[0]],p[t[3]]-p[t[0]])/6.0);
    }
}

/// Finds the indices of all tetrahedra in the ball of center ind_ta and of radius dist(ind_ta, ind_tb)
template<class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::getTetraInBall(const TetraID ind_ta, const TetraID ind_tb,
        sofa::helper::vector<unsigned int> &indices) const
{
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    const Tetrahedron ta=this->m_topology->getTetrahedron(ind_ta);
    const Tetrahedron tb=this->m_topology->getTetrahedron(ind_tb);

    const typename DataTypes::Coord& ca=(vect_c[ta[0]]+vect_c[ta[1]]+vect_c[ta[2]]+vect_c[ta[3]])*0.25;
    const typename DataTypes::Coord& cb=(vect_c[tb[0]]+vect_c[tb[1]]+vect_c[tb[2]]+vect_c[tb[3]])*0.25;
    Vec<3,Real> pa;
    Vec<3,Real> pb;
    pa[0] = (Real) (ca[0]);
    pa[1] = (Real) (ca[1]);
    pa[2] = (Real) (ca[2]);
    pb[0] = (Real) (cb[0]);
    pb[1] = (Real) (cb[1]);
    pb[2] = (Real) (cb[2]);

    Real d = (pa-pb)*(pa-pb);

    getTetraInBall(ind_ta, d, indices);
}

/// Finds the indices of all tetrahedra in the ball of center ind_ta and of radius dist(ind_ta, ind_tb)
template<class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::getTetraInBall(const TetraID ind_ta, Real r,
        sofa::helper::vector<unsigned int> &indices) const
{
    Real d = r;
    const Tetrahedron ta=this->m_topology->getTetrahedron(ind_ta);
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());
    const typename DataTypes::Coord& ca=(vect_c[ta[0]]+vect_c[ta[1]]+vect_c[ta[2]]+vect_c[ta[3]])*0.25;

    Vec<3,Real> pa;
    pa[0] = (Real) (ca[0]);
    pa[1] = (Real) (ca[1]);
    pa[2] = (Real) (ca[2]);

    unsigned int t_test=ind_ta;
    indices.push_back(t_test);

    std::map<unsigned int, unsigned int> IndexMap;
    IndexMap.clear();
    IndexMap[t_test]=0;

    sofa::helper::vector<unsigned int> ind2test;
    ind2test.push_back(t_test);
    sofa::helper::vector<unsigned int> ind2ask;
    ind2ask.push_back(t_test);

    while(ind2test.size()>0)
    {
        ind2test.clear();
        for (unsigned int t=0; t<ind2ask.size(); t++)
        {
            unsigned int ind_t = ind2ask[t];
            sofa::component::topology::TrianglesInTetrahedron adjacent_triangles = this->m_topology->getTrianglesInTetrahedron(ind_t);

            for (unsigned int i=0; i<adjacent_triangles.size(); i++)
            {
                sofa::helper::vector< unsigned int > tetrahedra_to_remove = this->m_topology->getTetrahedraAroundTriangle(adjacent_triangles[i]);

                if(tetrahedra_to_remove.size()==2)
                {
                    if(tetrahedra_to_remove[0]==ind_t)
                    {
                        t_test=tetrahedra_to_remove[1];
                    }
                    else
                    {
                        t_test=tetrahedra_to_remove[0];
                    }

                    std::map<unsigned int, unsigned int>::iterator iter_1 = IndexMap.find(t_test);
                    if(iter_1 == IndexMap.end())
                    {
                        IndexMap[t_test]=0;

                        const Tetrahedron tc=this->m_topology->getTetrahedron(t_test);
                        const typename DataTypes::Coord& cc = (vect_c[tc[0]]
                                + vect_c[tc[1]]
                                + vect_c[tc[2]]
                                + vect_c[tc[3]]) * 0.25;
                        Vec<3,Real> pc;
                        pc[0] = (Real) (cc[0]);
                        pc[1] = (Real) (cc[1]);
                        pc[2] = (Real) (cc[2]);

                        Real d_test = (pa-pc)*(pa-pc);

                        if(d_test<d)
                        {
                            ind2test.push_back(t_test);
                            indices.push_back(t_test);
                        }
                    }
                }
            }
        }

        ind2ask.clear();
        for (unsigned int t=0; t<ind2test.size(); t++)
        {
            ind2ask.push_back(ind2test[t]);
        }
    }

    return;
}

/// Finds the indices of all tetrahedra in the ball of center c and of radius r
template<class DataTypes>
void TetrahedronSetGeometryAlgorithms< DataTypes >::getTetraInBall(const Coord& c, Real r,
        sofa::helper::vector<unsigned int> &indices) const
{
    TetraID ind_ta = core::componentmodel::topology::BaseMeshTopology::InvalidID;
    Vec<3,Real> pa;
    pa[0] = (Real) (c[0]);
    pa[1] = (Real) (c[1]);
    pa[2] = (Real) (c[2]);
    for(int i = 0; i < this->m_topology->getNbTetrahedra(); ++i)
    {
        if(isPointInTetrahedron(i, pa))
        {
            ind_ta = i;
            break;
        }
    }
    if(ind_ta == core::componentmodel::topology::BaseMeshTopology::InvalidID)
        std::cout << "ERROR: Can't find the seed" << std::endl;
    Real d = r;
//		const Tetrahedron &ta=this->m_topology->getTetrahedron(ind_ta);
    const typename DataTypes::VecCoord& vect_c = *(this->object->getX());

    unsigned int t_test=ind_ta;
    indices.push_back(t_test);

    std::map<unsigned int, unsigned int> IndexMap;
    IndexMap.clear();
    IndexMap[t_test]=0;

    sofa::helper::vector<unsigned int> ind2test;
    ind2test.push_back(t_test);
    sofa::helper::vector<unsigned int> ind2ask;
    ind2ask.push_back(t_test);

    while(ind2test.size()>0)
    {
        ind2test.clear();
        for (unsigned int t=0; t<ind2ask.size(); t++)
        {
            unsigned int ind_t = ind2ask[t];
            sofa::component::topology::TrianglesInTetrahedron adjacent_triangles = this->m_topology->getTrianglesInTetrahedron(ind_t);

            for (unsigned int i=0; i<adjacent_triangles.size(); i++)
            {
                sofa::helper::vector< unsigned int > tetrahedra_to_remove = this->m_topology->getTetrahedraAroundTriangle(adjacent_triangles[i]);

                if(tetrahedra_to_remove.size()==2)
                {
                    if(tetrahedra_to_remove[0]==ind_t)
                    {
                        t_test=tetrahedra_to_remove[1];
                    }
                    else
                    {
                        t_test=tetrahedra_to_remove[0];
                    }

                    std::map<unsigned int, unsigned int>::iterator iter_1 = IndexMap.find(t_test);
                    if(iter_1 == IndexMap.end())
                    {
                        IndexMap[t_test]=0;

                        const Tetrahedron tc=this->m_topology->getTetrahedron(t_test);
                        const typename DataTypes::Coord& cc = (vect_c[tc[0]]
                                + vect_c[tc[1]]
                                + vect_c[tc[2]]
                                + vect_c[tc[3]]) * 0.25;
                        Vec<3,Real> pc;
                        pc[0] = (Real) (cc[0]);
                        pc[1] = (Real) (cc[1]);
                        pc[2] = (Real) (cc[2]);

                        Real d_test = (pa-pc)*(pa-pc);

                        if(d_test<d)
                        {
                            ind2test.push_back(t_test);
                            indices.push_back(t_test);
                        }
                    }
                }
            }
        }

        ind2ask.clear();
        for (unsigned int t=0; t<ind2test.size(); t++)
        {
            ind2ask.push_back(ind2test[t]);
        }
    }

    return;
}


/// Write the current mesh into a msh file
template <typename DataTypes>
void TetrahedronSetGeometryAlgorithms<DataTypes>::writeMSHfile(const char *filename) const
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

    const sofa::helper::vector<Tetrahedron> &tea = this->m_topology->getTetrahedra();

    myfile << tea.size() <<"\n";

    for (unsigned int i=0; i<tea.size(); ++i)
    {
        myfile << i+1 << " 4 1 1 4 " << tea[i][0]+1 << " " << tea[i][1]+1 << " " << tea[i][2]+1 << " " << tea[i][3]+1 <<"\n";
    }

    myfile << "$ENDELM\n";

    myfile.close();
}




template<class DataTypes>
void TetrahedronSetGeometryAlgorithms<DataTypes>::draw()
{
    TriangleSetGeometryAlgorithms<DataTypes>::draw();

    //Draw tetra indices
    if (debugViewTetrahedraIndices.getValue())
    {
        Mat<4,4, GLfloat> modelviewM;
        const VecCoord& coords = *(this->object->getX());
        glColor3f(1.0,1.0,0.0);
        glDisable(GL_LIGHTING);
        float scale = PointSetGeometryAlgorithms<DataTypes>::PointIndicesScale;

        //for tetra:
        scale = scale/2;

        const sofa::helper::vector<Tetrahedron> &tetraArray = this->m_topology->getTetrahedra();

        for (unsigned int i =0; i<tetraArray.size(); i++)
        {

            Tetrahedron the_tetra = tetraArray[i];
            Coord baryCoord;
            Coord vertex1 = coords[ the_tetra[0] ];
            Coord vertex2 = coords[ the_tetra[1] ];
            Coord vertex3 = coords[ the_tetra[2] ];
            Coord vertex4 = coords[ the_tetra[3] ];

            for (unsigned int k = 0; k<3; k++)
                baryCoord[k] = (vertex1[k]+vertex2[k]+vertex3[k]+vertex4[k])/4;

            std::ostringstream oss;
            oss << i;
            std::string tmp = oss.str();
            const char* s = tmp.c_str();
            glPushMatrix();

            glTranslatef(baryCoord[0], baryCoord[1], baryCoord[2]);
            glScalef(scale,scale,scale);

            // Makes text always face the viewer by removing the scene rotation
            // get the current modelview matrix
            glGetFloatv(GL_MODELVIEW_MATRIX , modelviewM.ptr() );
            modelviewM.transpose();

            Vec3d temp(baryCoord[0], baryCoord[1], baryCoord[2]);
            temp = modelviewM.transform(temp);

            //glLoadMatrixf(modelview);
            glLoadIdentity();

            glTranslatef(temp[0], temp[1], temp[2]);
            glScalef(scale,scale,scale);

            while(*s)
            {
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *s);
                s++;
            }

            glPopMatrix();

        }
    }


    // Draw Tetra
    if (_draw.getValue())
    {
        const sofa::helper::vector<Tetrahedron> &tetraArray = this->m_topology->getTetrahedra();

        if (!tetraArray.empty())
        {
            glDisable(GL_LIGHTING);
            glColor3f(1.0,1.0,0.0);
            glBegin(GL_LINES);
            const VecCoord& coords = *(this->object->getX());

            for (unsigned int i = 0; i<tetraArray.size(); i++)
            {
                const Tetrahedron& tet = tetraArray[i];
                sofa::helper::vector <Coord> tetraCoord;

                for (unsigned int j = 0; j<4; j++)
                    tetraCoord.push_back (coords[tet[j]]);

                for (unsigned int j = 0; j<4; j++)
                {
                    glVertex3d(tetraCoord[j][0], tetraCoord[j][1], tetraCoord[j][2]);
                    glVertex3d(tetraCoord[(j+1)%4][0], tetraCoord[(j+1)%4][1], tetraCoord[(j+1)%4][2]);
                }

                glVertex3d(tetraCoord[0][0], tetraCoord[0][1], tetraCoord[0][2]);
                glVertex3d(tetraCoord[2][0], tetraCoord[2][1], tetraCoord[2][2]);

                glVertex3d(tetraCoord[1][0], tetraCoord[1][1], tetraCoord[1][2]);
                glVertex3d(tetraCoord[3][0], tetraCoord[3][1], tetraCoord[3][2]);
            }
            glEnd();
        }
    }

}



} // namespace topology

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENTS_TETEAHEDRONSETGEOMETRYALGORITHMS_INL
