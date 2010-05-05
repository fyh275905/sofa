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
#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_INL
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_PLANEFORCEFIELD_INL

#include <sofa/core/behavior/ForceField.inl>
#include <sofa/simulation/common/Simulation.h>
#include "PlaneForceField.h"
#include <sofa/helper/system/config.h>
#include <sofa/helper/accessor.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/helper/gl/template.h>
#include <assert.h>
#include <iostream>



namespace sofa
{

namespace component
{

namespace forcefield
{


template<class DataTypes>
void PlaneForceField<DataTypes>::addForce(VecDeriv& vf1, const VecCoord& vp1, const VecDeriv& vv1)
{
    helper::WriteAccessor<VecDeriv> f1 = vf1;
    helper::ReadAccessor<VecCoord> p1 = vp1;
    helper::ReadAccessor<VecDeriv> v1 = vv1;

    //this->dfdd.resize(p1.size());
    this->contacts.clear();
    f1.resize(p1.size());

    unsigned int ibegin = 0;
    unsigned int iend = p1.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    for (unsigned int i=ibegin; i<iend; i++)
    {
        Real d = p1[i]*planeNormal.getValue()-planeD.getValue();
        if (d<0)
        {
            //serr<<"PlaneForceField<DataTypes>::addForce, d = "<<d<<sendl;
            Real forceIntensity = -this->stiffness.getValue()*d;
            //serr<<"PlaneForceField<DataTypes>::addForce, stiffness = "<<stiffness.getValue()<<sendl;
            Real dampingIntensity = -this->damping.getValue()*d;
            //serr<<"PlaneForceField<DataTypes>::addForce, dampingIntensity = "<<dampingIntensity<<sendl;
            Deriv force = planeNormal.getValue()*forceIntensity - v1[i]*dampingIntensity;
            //serr<<"PlaneForceField<DataTypes>::addForce, force = "<<force<<sendl;
            f1[i]+=force;
            //this->dfdd[i] = -this->stiffness;
            this->contacts.push_back(i);
        }
    }
}

template<class DataTypes>
void PlaneForceField<DataTypes>::addDForce(VecDeriv& vf1, const VecDeriv& vdx1, double kFactor, double /*bFactor*/)
{
    helper::WriteAccessor<VecDeriv> f1 = vf1;
    helper::ReadAccessor<VecDeriv> dx1 = vdx1;

    f1.resize(dx1.size());
    const Real fact = (Real)(-this->stiffness.getValue()*kFactor);
    for (unsigned int i=0; i<this->contacts.size(); i++)
    {
        unsigned int p = this->contacts[i];
        assert(p<dx1.size());
        f1[p] += planeNormal.getValue() * (fact * (dx1[p]*planeNormal.getValue()));
    }
}

template<class DataTypes>
void PlaneForceField<DataTypes>::addKToMatrix(sofa::defaulttype::BaseMatrix *mat, SReal kFactor, unsigned int &offset)
{
    const Real fact = (Real)(-this->stiffness.getValue()*kFactor);
    const Deriv& normal = planeNormal.getValue();
    for (unsigned int i=0; i<this->contacts.size(); i++)
    {
        unsigned int p = this->contacts[i];
        for (int l=0; l<Deriv::static_size; ++l)
            for (int c=0; c<Deriv::static_size; ++c)
            {
                SReal coef = normal[l] * fact * normal[c];
                mat->add(offset + p*Deriv::static_size + l, offset + p*Deriv::static_size + c, coef);
            }
    }
}
template<class DataTypes>
void PlaneForceField<DataTypes>::updateStiffness( const VecCoord& vx )
{
    helper::ReadAccessor<VecCoord> x = vx;

    this->contacts.clear();

    unsigned int ibegin = 0;
    unsigned int iend = x.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    for (unsigned int i=ibegin; i<iend; i++)
    {
        Real d = x[i]*planeNormal.getValue()-planeD.getValue();
        if (d<0)
        {
            this->contacts.push_back(i);
        }
    }
}


template <class DataTypes>
double PlaneForceField<DataTypes>::getPotentialEnergy(const VecCoord&) const
{
    serr<<"PlaneForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}


// Rotate the plane. Note that the rotation is only applied on the 3 first coordinates
template<class DataTypes>
void PlaneForceField<DataTypes>::rotate( Deriv axe, Real angle )
{
    defaulttype::Vec3d axe3d(1,1,1); axe3d = axe;
    defaulttype::Vec3d normal3d; normal3d = planeNormal.getValue();
    defaulttype::Vec3d v = normal3d.cross(axe3d);
    v.normalize();
    v = normal3d * cos ( angle ) + v * sin ( angle );
    *planeNormal.beginEdit() = v;
    planeNormal.endEdit();
}


template<class DataTypes>
void PlaneForceField<DataTypes>::draw()
{
    if (!this->getContext()->getShowForceFields()) return;
    drawPlane();
}


template<class DataTypes>
void PlaneForceField<DataTypes>::drawPlane(float size)
{
    if (size == 0.0f) size = (float)drawSize.getValue();

    helper::ReadAccessor<VecCoord> p1 = *this->mstate->getX();

    defaulttype::Vec3d normal; normal = planeNormal.getValue();

    // find a first vector inside the plane
    defaulttype::Vec3d v1;
    if( 0.0 != normal[0] ) v1 = defaulttype::Vec3d(-normal[1]/normal[0], 1.0, 0.0);
    else if ( 0.0 != normal[1] ) v1 = defaulttype::Vec3d(1.0, -normal[0]/normal[1],0.0);
    else if ( 0.0 != normal[2] ) v1 = defaulttype::Vec3d(1.0, 0.0, -normal[0]/normal[2]);
    v1.normalize();
    // find a second vector inside the plane and orthogonal to the first
    defaulttype::Vec3d v2;
    v2 = v1.cross(normal);
    v2.normalize();

    defaulttype::Vec3d center = normal*planeD.getValue();
    defaulttype::Vec3d corners[4];
    corners[0] = center-v1*size-v2*size;
    corners[1] = center+v1*size-v2*size;
    corners[2] = center+v1*size+v2*size;
    corners[3] = center-v1*size+v2*size;


    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);


    std::vector< defaulttype::Vector3 > points;

    points.push_back(corners[0]);
    points.push_back(corners[1]);
    points.push_back(corners[2]);

    points.push_back(corners[0]);
    points.push_back(corners[2]);
    points.push_back(corners[3]);

    simulation::getSimulation()->DrawUtility.setPolygonMode(2,false); //Cull Front face

    simulation::getSimulation()->DrawUtility.drawTriangles(points, defaulttype::Vec<4,float>(color.getValue()[0],color.getValue()[1],color.getValue()[2],0.5));
    simulation::getSimulation()->DrawUtility.setPolygonMode(0,false); //No Culling
    glDisable(GL_CULL_FACE);

    std::vector< defaulttype::Vector3 > pointsLine;
    // lines for points penetrating the plane

    unsigned int ibegin = 0;
    unsigned int iend = p1.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;


    defaulttype::Vector3 point1,point2;
    unsigned int sizePoints= (Coord::static_size <=3)?Coord::static_size:3;
    for (unsigned int i=ibegin; i<iend; i++)
    {
        Real d = p1[i]*planeNormal.getValue()-planeD.getValue();
        Coord p2 = p1[i];
        p2 += planeNormal.getValue()*(-d);
        if (d<0)
        {
            for (unsigned int s=0; s<sizePoints; ++s)
            {
                point1[s] = p1[i][s];
                point2[s] = p2[s];
            }
        }
        pointsLine.push_back(point1);
        pointsLine.push_back(point2);
    }
    simulation::getSimulation()->DrawUtility.drawLines(pointsLine, 1, defaulttype::Vec<4,float>(1,0,0,1));
}

template <class DataTypes>
bool PlaneForceField<DataTypes>::addBBox(double* minBBox, double* maxBBox)
{
    if (!bDraw.getValue()) return false;

    defaulttype::Vec3d normal; normal = planeNormal.getValue();
    double size=10.0;

    // find a first vector inside the plane
    defaulttype::Vec3d v1;
    if( 0.0 != normal[0] ) v1 = defaulttype::Vec3d(-normal[1]/normal[0], 1.0, 0.0);
    else if ( 0.0 != normal[1] ) v1 = defaulttype::Vec3d(1.0, -normal[0]/normal[1],0.0);
    else if ( 0.0 != normal[2] ) v1 = defaulttype::Vec3d(1.0, 0.0, -normal[0]/normal[2]);
    v1.normalize();
    // find a second vector inside the plane and orthogonal to the first
    defaulttype::Vec3d v2;
    v2 = v1.cross(normal);
    v2.normalize();

    defaulttype::Vec3d center = normal*planeD.getValue();
    defaulttype::Vec3d corners[4];
    corners[0] = center-v1*size-v2*size;
    corners[1] = center+v1*size-v2*size;
    corners[2] = center+v1*size+v2*size;
    corners[3] = center-v1*size+v2*size;

    for (unsigned int i=0; i<4; i++)
    {
        for (int c=0; c<3; c++)
        {
            if (corners[i][c] > maxBBox[c]) maxBBox[c] = corners[i][c];
            if (corners[i][c] < minBBox[c]) minBBox[c] = corners[i][c];
        }
    }
    return true;
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
