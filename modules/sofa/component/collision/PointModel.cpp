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
#include <sofa/helper/system/config.h>
#include <sofa/helper/proximity.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/componentmodel/collision/Intersection.inl>
#include <iostream>
#include <algorithm>




#include <sofa/component/collision/PointModel.h>
#include <sofa/component/collision/CubeModel.h>
#include <sofa/core/ObjectFactory.h>
#include <vector>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/componentmodel/collision/Intersection.inl>

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>

#include <sofa/simulation/tree/Simulation.h>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::collision;
using namespace helper;

SOFA_DECL_CLASS(Point)

int PointModelClass = core::RegisterObject("Collision model which represents a set of points")
        .add< PointModel >()
        .addAlias("Point")
// .addAlias("PointModel")
        .addAlias("PointMesh")
        .addAlias("PointSet")
        ;

PointModel::PointModel()
    : mstate(NULL)
    , computeNormals( initData(&computeNormals, false, "computeNormals", "activate computation of normal vectors (required for some collision detection algorithms)") )
{
}

void PointModel::resize(int size)
{
    this->core::CollisionModel::resize(size);
}

void PointModel::init()
{
    this->CollisionModel::init();
    mstate = dynamic_cast< core::componentmodel::behavior::MechanicalState<Vec3Types>* > (getContext()->getMechanicalState());

    if (mstate==NULL)
    {
        serr<<"ERROR: PointModel requires a Vec3 Mechanical Model" << sendl;
        return;
    }

    const int npoints = mstate->getX()->size();
    resize(npoints);
    if (computeNormals.getValue()) updateNormals();
}

void PointModel::draw(int index)
{
    Point t(this,index);
    glBegin(GL_POINTS);
    helper::gl::glVertexT(t.p());
    glEnd();
    if ((unsigned)index < normals.size())
    {
        glBegin(GL_LINES);
        helper::gl::glVertexT(t.p());
        helper::gl::glVertexT(t.p()+normals[index]*0.1f);
        glEnd();
    }
}

void PointModel::draw()
{
    if (getContext()->getShowCollisionModels())
    {
        if (getContext()->getShowWireFrame())
            simulation::tree::getSimulation()->DrawUtility.setPolygonMode(0,true);


        // Check topological modifications
        const int npoints = mstate->getX()->size();
        if (npoints != size)
        {
            resize(npoints);
        }

        std::vector< Vector3 > pointsP;
        std::vector< Vector3 > pointsL;
        for (int i = 0; i < size; i++)
        {
            Point t(this,i);
            pointsP.push_back(t.p());
            if ((unsigned)i < normals.size())
            {
                pointsL.push_back(t.p());
                pointsL.push_back(t.p()+normals[i]*0.1f);
            }
        }
        simulation::tree::getSimulation()->DrawUtility.drawPoints(pointsP, 3, Vec<4,float>(getColor4f()));
        simulation::tree::getSimulation()->DrawUtility.drawLines(pointsL, 1, Vec<4,float>(getColor4f()));

        if (getContext()->getShowWireFrame())
            simulation::tree::getSimulation()->DrawUtility.setPolygonMode(0,false);
    }
    if (getPrevious()!=NULL && getContext()->getShowBoundingCollisionModels())
        getPrevious()->draw();
}

bool PointModel::canCollideWithElement(int index, CollisionModel* model2, int index2)
{
    if (!this->bSelfCollision.getValue()) return true;
    if (this->getContext() != model2->getContext()) return true;
    if (model2 == this)
    {
        //sout << "point self test "<<index<<" - "<<index2<<sendl;
        return index < index2-2; // || index > index2+1;
    }
    else
        return model2->canCollideWithElement(index2, this, index);
}

void PointModel::computeBoundingTree(int maxDepth)
{
    CubeModel* cubeModel = createPrevious<CubeModel>();
    const int npoints = mstate->getX()->size();
    bool updated = false;
    if (npoints != size)
    {
        resize(npoints);
        updated = true;
    }
    if (updated) cubeModel->resize(0);
    if (!isMoving() && !cubeModel->empty() && !updated) return; // No need to recompute BBox if immobile

    if (computeNormals.getValue()) updateNormals();

    cubeModel->resize(size);
    if (!empty())
    {
        //VecCoord& x = *mstate->getX();
        for (int i=0; i<size; i++)
        {
            Point p(this,i);
            const Vector3& pt = p.p();
            cubeModel->setParentOf(i, pt, pt);
        }
        cubeModel->computeBoundingTree(maxDepth);
    }
}

void PointModel::computeContinuousBoundingTree(double dt, int maxDepth)
{
    CubeModel* cubeModel = createPrevious<CubeModel>();
    const int npoints = mstate->getX()->size();
    bool updated = false;
    if (npoints != size)
    {
        resize(npoints);
        updated = true;
    }
    if (!isMoving() && !cubeModel->empty() && !updated) return; // No need to recompute BBox if immobile

    if (computeNormals.getValue()) updateNormals();

    Vector3 minElem, maxElem;

    cubeModel->resize(size);
    if (!empty())
    {
        //VecCoord& x = *mstate->getX();
        //VecDeriv& v = *mstate->getV();
        for (int i=0; i<size; i++)
        {
            Point p(this,i);
            const Vector3& pt = p.p();
            const Vector3 ptv = pt + p.v()*dt;

            for (int c = 0; c < 3; c++)
            {
                minElem[c] = pt[c];
                maxElem[c] = pt[c];
                if (ptv[c] > maxElem[c]) maxElem[c] = ptv[c];
                else if (ptv[c] < minElem[c]) minElem[c] = ptv[c];
            }
            cubeModel->setParentOf(i, minElem, maxElem);
        }
        cubeModel->computeBoundingTree(maxDepth);
    }
}

void PointModel::updateNormals()
{
    const VecCoord& x = *mstate->getX();
    int n = x.size();
    normals.resize(n);
    for (int i=0; i<n; ++i)
    {
        normals[i].clear();
    }
    core::componentmodel::topology::BaseMeshTopology* mesh = getContext()->getMeshTopology();
    if (mesh->getNbTetras()+mesh->getNbHexas() > 0)
    {
        if (mesh->getNbTetras()>0)
        {
            const core::componentmodel::topology::BaseMeshTopology::SeqTetras &elems = mesh->getTetras();
            for (unsigned int i=0; i < elems.size(); ++i)
            {
                const core::componentmodel::topology::BaseMeshTopology::Tetra &e = elems[i];
                const Coord& p1 = x[e[0]];
                const Coord& p2 = x[e[1]];
                const Coord& p3 = x[e[2]];
                const Coord& p4 = x[e[3]];
                Coord& n1 = normals[e[0]];
                Coord& n2 = normals[e[1]];
                Coord& n3 = normals[e[2]];
                Coord& n4 = normals[e[3]];
                Coord n;
                n = cross(p3-p1,p2-p1); n.normalize();
                n1 += n;
                n2 += n;
                n3 += n;
                n = cross(p4-p1,p3-p1); n.normalize();
                n1 += n;
                n3 += n;
                n4 += n;
                n = cross(p2-p1,p4-p1); n.normalize();
                n1 += n;
                n4 += n;
                n2 += n;
                n = cross(p3-p2,p4-p2); n.normalize();
                n2 += n;
                n4 += n;
                n3 += n;
            }
        }
        /// @TODO Hexas
    }
    else if (mesh->getNbTriangles()+mesh->getNbQuads() > 0)
    {
        if (mesh->getNbTriangles()>0)
        {
            const core::componentmodel::topology::BaseMeshTopology::SeqTriangles &elems = mesh->getTriangles();
            for (unsigned int i=0; i < elems.size(); ++i)
            {
                const core::componentmodel::topology::BaseMeshTopology::Triangle &e = elems[i];
                const Coord& p1 = x[e[0]];
                const Coord& p2 = x[e[1]];
                const Coord& p3 = x[e[2]];
                Coord& n1 = normals[e[0]];
                Coord& n2 = normals[e[1]];
                Coord& n3 = normals[e[2]];
                Coord n;
                n = cross(p2-p1,p3-p1); n.normalize();
                n1 += n;
                n2 += n;
                n3 += n;
            }
        }
        if (mesh->getNbQuads()>0)
        {
            const core::componentmodel::topology::BaseMeshTopology::SeqQuads &elems = mesh->getQuads();
            for (unsigned int i=0; i < elems.size(); ++i)
            {
                const core::componentmodel::topology::BaseMeshTopology::Quad &e = elems[i];
                const Coord& p1 = x[e[0]];
                const Coord& p2 = x[e[1]];
                const Coord& p3 = x[e[2]];
                const Coord& p4 = x[e[3]];
                Coord& n1 = normals[e[0]];
                Coord& n2 = normals[e[1]];
                Coord& n3 = normals[e[2]];
                Coord& n4 = normals[e[3]];
                Coord n;
                n = cross(p3-p1,p4-p2); n.normalize();
                n1 += n;
                n2 += n;
                n3 += n;
                n4 += n;
            }
        }
    }
    for (int i=0; i<n; ++i)
    {
        SReal l = normals[i].norm();
        if (l > 1.0e-3)
            normals[i] *= 1/l;
        else
            normals[i].clear();
    }
}


bool Point::testLMD(const Vector3 &PQ, double &coneFactor, double &coneExtension)
{
    Vector3 pt = p();

    sofa::core::componentmodel::topology::BaseMeshTopology* mesh = model->getMeshTopology();
    helper::vector<Vector3> x = (*model->mstate->getX());

    const helper::vector <unsigned int>& triangleVertexShell = mesh->getTriangleVertexShell(index);
    const helper::vector <unsigned int>& edgeVertexShell = mesh->getEdgeVertexShell(index);


    Vector3 nMean;

    for (unsigned int i=0; i<triangleVertexShell.size(); i++)
    {
        unsigned int t = triangleVertexShell[i];
        const fixed_array<unsigned int,3>& ptr = mesh->getTriangle(t);
        Vector3 nCur = (x[ptr[1]]-x[ptr[0]]).cross(x[ptr[2]]-x[ptr[0]]);
        nCur.normalize();
        nMean += nCur;
    }

    if (triangleVertexShell.size()==0)
    {
        for (unsigned int i=0; i<edgeVertexShell.size(); i++)
        {
            unsigned int e = edgeVertexShell[i];
            const fixed_array<unsigned int,2>& ped = mesh->getEdge(e);
            Vector3 l = (pt - x[ped[0]]) + (pt - x[ped[1]]);
            l.normalize();
            nMean += l;
        }
    }

    if (nMean.norm()> 0.0000000001)
        nMean.normalize();


    for (unsigned int i=0; i<edgeVertexShell.size(); i++)
    {
        unsigned int e = edgeVertexShell[i];
        const fixed_array<unsigned int,2>& ped = mesh->getEdge(e);
        Vector3 l = (pt - x[ped[0]]) + (pt - x[ped[1]]);
        l.normalize();
        double computedAngleCone = dot(nMean , l) * coneFactor;
        if (computedAngleCone<0)
            computedAngleCone=0.0;
        computedAngleCone+=coneExtension;
        if (dot(l , PQ) < -computedAngleCone*PQ.norm())
            return false;
    }
    return true;


}








} // namespace collision

} // namespace component

} // namespace sofa

