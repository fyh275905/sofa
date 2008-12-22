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
#ifndef SOFA_COMPONENT_MASS_UNIFORMMASS_INL
#define SOFA_COMPONENT_MASS_UNIFORMMASS_INL

#include <sofa/component/mass/UniformMass.h>
#include <sofa/core/componentmodel/behavior/Mass.inl>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/helper/gl/template.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/component/mass/AddMToMatrixFunctor.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/helper/gl/DrawManager.h>
#include <iostream>
#include <string.h>




namespace sofa
{

namespace component
{

namespace mass
{

using namespace sofa::defaulttype;


template <class DataTypes, class MassType>
UniformMass<DataTypes, MassType>::UniformMass()
    : mass( initData(&mass, MassType(1.0f), "mass", "Mass of each particle") )
    , totalMass( initData(&totalMass, 0.0, "totalmass", "Sum of the particles' masses") )
    , filenameMass( initData(&filenameMass, "filename", "Rigid file to load the mass parameters") )
    , showCenterOfGravity( initData(&showCenterOfGravity, false, "showGravityCenter", "display the center of gravity of the system" ) )
    , showAxisSize( initData(&showAxisSize, 1.0f, "showAxisSizeFactor", "factor length of the axis displayed (only used for rigids)" ) )
    , compute_mapping_inertia( initData(&compute_mapping_inertia, true, "compute_mapping_inertia", "to be used if the mass is placed under a mapping" ) )
    , showInitialCenterOfGravity( initData(&showInitialCenterOfGravity, false, "showInitialCenterOfGravity", "display the initial center of gravity of the system" ) )
    , showX0( initData(&showX0, false, "showX0", "display the rest positions" ) )
    , localRange( initData(&localRange, defaulttype::Vec<2,int>(-1,-1), "localRange", "optional range of local DOF indices. Any computation involving only indices outside of this range are discarded (useful for parallelization using mesh partitionning)" ) )
{}

template <class DataTypes, class MassType>
UniformMass<DataTypes, MassType>::~UniformMass()
{}

template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::setMass(const MassType& m)
{
    this->mass.setValue(m);
}

template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::setTotalMass(double m)
{
    this->totalMass.setValue(m);
}

template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::reinit()
{
    if (this->totalMass.getValue()>0 && this->mstate!=NULL)
    {
        MassType* m = this->mass.beginEdit();
        *m = ((typename DataTypes::Real)this->totalMass.getValue() / this->mstate->getX()->size());
        this->mass.endEdit();
    }
    else
    {
        this->totalMass.setValue(  this->mstate->getX()->size()*this->mass.getValue());
    }
}
template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::init()
{
    loadRigidMass(filenameMass.getValue());
    if (filenameMass.getValue().empty()) filenameMass.setDisplayed(false);
    this->core::componentmodel::behavior::Mass<DataTypes>::init();
    reinit();
}

// -- Mass interface
template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMDx(VecDeriv& res, const VecDeriv& dx, double factor)
{
    unsigned int ibegin = 0;
    unsigned int iend = dx.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    MassType m = mass.getValue();
    if (factor != 1.0)
        m *= (typename DataTypes::Real)factor;

    for (unsigned int i=ibegin; i<iend; i++)
    {
        res[i] += dx[i] * m;
        //serr<<"dx[i] = "<<dx[i]<<", m = "<<m<<", dx[i] * m = "<<dx[i] * m<<sendl;
    }
}

template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::accFromF(VecDeriv& a, const VecDeriv& f)
{

    unsigned int ibegin = 0;
    unsigned int iend = f.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    const MassType& m = mass.getValue();
    for (unsigned int i=ibegin; i<iend; i++)
    {
        a[i] = f[i] / m;
        // serr<<"f[i] = "<<f[i]<<", m = "<<m<<", f[i] / m = "<<f[i] / m<<sendl;
    }
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMDxToVector(defaulttype::BaseVector * /*resVect*/, const VecDeriv* /*dx*/, SReal /*mFact*/, unsigned int& /*offset*/)
{

}

template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addGravityToV(double dt)
{
    if (this->mstate)
    {
        VecDeriv& v = *this->mstate->getV();
        const SReal* g = this->getContext()->getLocalGravity().ptr();
        Deriv theGravity;
        DataTypes::set( theGravity, g[0], g[1], g[2]);
        Deriv hg = theGravity * (Real)dt;
        if (this->f_printLog.getValue())
            serr << "UniformMass::addGravityToV hg = "<<theGravity<<"*"<<dt<<"="<<hg<<sendl;
        for (unsigned int i=0; i<v.size(); i++)
        {
            v[i] += hg;
        }
    }
}

template <class DataTypes, class MassType>
#ifdef SOFA_SUPPORT_MOVING_FRAMES
void UniformMass<DataTypes, MassType>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v)
#else
void UniformMass<DataTypes, MassType>::addForce(VecDeriv& f, const VecCoord& /*x*/, const VecDeriv& /*v*/)
#endif
{

    //if gravity was added separately (in solver's "solve" method), then nothing to do here
    if(this->m_separateGravity.getValue())
        return;

    unsigned int ibegin = 0;
    unsigned int iend = f.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    // weight
    const SReal* g = this->getContext()->getLocalGravity().ptr();
    Deriv theGravity;
    DataTypes::set
    ( theGravity, g[0], g[1], g[2]);
    const MassType& m = mass.getValue();
    Deriv mg = theGravity * m;
    if (this->f_printLog.getValue())
        serr<<"UniformMass::addForce, mg = "<<mass<<" * "<<theGravity<<" = "<<mg<<sendl;





#ifdef SOFA_SUPPORT_MOVING_FRAMES
    // velocity-based stuff
    core::objectmodel::BaseContext::SpatialVector vframe = getContext()->getVelocityInWorld();
    core::objectmodel::BaseContext::Vec3 aframe = getContext()->getVelocityBasedLinearAccelerationInWorld() ;
//     serr<<"UniformMass<DataTypes, MassType>::computeForce(), vFrame in world coordinates = "<<vframe<<sendl;
    //serr<<"UniformMass<DataTypes, MassType>::computeForce(), aFrame in world coordinates = "<<aframe<<sendl;
//     serr<<"UniformMass<DataTypes, MassType>::computeForce(), getContext()->getLocalToWorld() = "<<getContext()->getPositionInWorld()<<sendl;

    // project back to local frame
    vframe = getContext()->getPositionInWorld() / vframe;
    aframe = getContext()->getPositionInWorld().backProjectVector( aframe );
//     serr<<"UniformMass<DataTypes, MassType>::computeForce(), vFrame in local coordinates= "<<vframe<<sendl;
//     serr<<"UniformMass<DataTypes, MassType>::computeForce(), aFrame in local coordinates= "<<aframe<<sendl;
//     serr<<"UniformMass<DataTypes, MassType>::computeForce(), mg in local coordinates= "<<mg<<sendl;
#endif


    // add weight and inertia force
    for (unsigned int i=ibegin; i<iend; i++)
    {
#ifdef SOFA_SUPPORT_MOVING_FRAMES
        f[i] += mg + core::componentmodel::behavior::inertiaForce(vframe,aframe,m,x[i],v[i]);
#else
        f[i] += mg;
#endif
        //serr<<"UniformMass<DataTypes, MassType>::computeForce(), vframe = "<<vframe<<", aframe = "<<aframe<<", x = "<<x[i]<<", v = "<<v[i]<<sendl;
        //serr<<"UniformMass<DataTypes, MassType>::computeForce() = "<<mg + Core::inertiaForce(vframe,aframe,mass,x[i],v[i])<<sendl;
    }

#ifdef SOFA_SUPPORT_MAPPED_MASS
    if (compute_mapping_inertia.getValue())
    {
        VecDeriv& acc =  *this->mstate->getDx();
        // add inertia force due to acceleration from the motion of the mapping (coriolis type force)
        if (acc.size() != f.size())
            return;
        for (unsigned int i=0; i<f.size(); i++)
        {
            Deriv coriolis = -acc[i]*m;
            f[i] += coriolis;
        }
    }
#endif
}

template <class DataTypes, class MassType>
double UniformMass<DataTypes, MassType>::getKineticEnergy( const VecDeriv& v )
{

    unsigned int ibegin = 0;
    unsigned int iend = v.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    double e=0;
    const MassType& m = mass.getValue();
    for (unsigned int i=ibegin; i<iend; i++)
    {
        e+= v[i]*m*v[i];
    }
    //serr<<"UniformMass<DataTypes, MassType>::getKineticEnergy = "<<e/2<<sendl;
    return e/2;
}

template <class DataTypes, class MassType>
double UniformMass<DataTypes, MassType>::getPotentialEnergy( const VecCoord& x )
{
    unsigned int ibegin = 0;
    unsigned int iend = x.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    double e = 0;
    const MassType& m = mass.getValue();
    // gravity
    Vec3d g ( this->getContext()->getLocalGravity() );
    Deriv theGravity;
    DataTypes::set
    ( theGravity, g[0], g[1], g[2]);
    Deriv mg = theGravity * m;
    //serr<<"UniformMass<DataTypes, MassType>::getPotentialEnergy, theGravity = "<<theGravity<<sendl;
    for (unsigned int i=ibegin; i<iend; i++)
    {
        /*        serr<<"UniformMass<DataTypes, MassType>::getPotentialEnergy, mass = "<<mass<<sendl;
                serr<<"UniformMass<DataTypes, MassType>::getPotentialEnergy, x = "<<x[i]<<sendl;
                serr<<"UniformMass<DataTypes, MassType>::getPotentialEnergy, remove "<<theGravity*mass*x[i]<<sendl;*/
        e -= mg*x[i];
    }
    return e;
}

/// Add Mass contribution to global Matrix assembling
template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::addMToMatrix(defaulttype::BaseMatrix * mat, double mFact, unsigned int &offset)
{
    const MassType& m = mass.getValue();
    const int N = defaulttype::DataTypeInfo<Deriv>::size();
    const unsigned int size = this->mstate->getSize();
    AddMToMatrixFunctor<Deriv,MassType> calc;
    for (unsigned int i=0; i<size; i++)
        calc(mat, m, offset + N*i, mFact);
}


template <class DataTypes, class MassType>
double UniformMass<DataTypes, MassType>::getElementMass(unsigned int )
{
    return (double)(mass.getValue());
}


template <class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::draw()
{
    if (!getContext()->getShowBehaviorModels())
        return;
    const VecCoord& x = *this->mstate->getX();

    unsigned int ibegin = 0;
    unsigned int iend = x.size();

    if (localRange.getValue()[0] >= 0)
        ibegin = localRange.getValue()[0];

    if (localRange.getValue()[1] >= 0 && (unsigned int)localRange.getValue()[1]+1 < iend)
        iend = localRange.getValue()[1]+1;

    //serr<<"UniformMass<DataTypes, MassType>::draw() "<<x<<sendl;


    std::vector<  Vector3 > points;
    std::vector< Vec<2,int> > indices;

    Coord gravityCenter;
    glDisable (GL_LIGHTING);
    glPointSize(2);
    for (unsigned int i=ibegin; i<iend; i++)
    {
        Vector3 p;
        for (unsigned int j=0; j< Coord::static_size; ++j)
            p[j] = x[i][j];

        points.push_back(p);
        gravityCenter += x[i];
    }
    simulation::tree::getSimulation()->DrawUtility.drawPoints(points, Vec<4,float>(1,1,1,1));

    if(showCenterOfGravity.getValue())
    {
        points.clear();
        glBegin (GL_LINES);
        glColor4f (1,1,0,1);
        gravityCenter /= x.size();
        for(unsigned int i=0 ; i<Coord::static_size ; i++)
        {
            Coord v;
            v[i] = showAxisSize.getValue();
            helper::gl::glVertexT(gravityCenter-v);
            helper::gl::glVertexT(gravityCenter+v);
        }
        glEnd();
    }
}

template <class DataTypes, class MassType>
bool UniformMass<DataTypes, MassType>::addBBox(double* minBBox, double* maxBBox)
{
    const VecCoord& x = *this->mstate->getX();
    for (unsigned int i=0; i<x.size(); i++)
    {
        //const Coord& p = x[i];
        Real p[3] = {0.0, 0.0, 0.0};
        DataTypes::get(p[0],p[1],p[2],x[i]);
        for (int c=0; c<3; c++)
        {
            if (p[c] > maxBBox[c]) maxBBox[c] = p[c];
            if (p[c] < minBBox[c]) minBBox[c] = p[c];
        }
    }
    return true;
}

template<class DataTypes, class MassType>
void UniformMass<DataTypes, MassType>::loadRigidMass(std::string )
{
    //If the template is not rigid, we hide the Data filenameMass, to avoid confusion.
    filenameMass.setDisplayed(false);
    this->mass.setDisplayed(false);
}


//Specialization for rigids
#ifndef SOFA_FLOAT
template<>
void UniformMass<Rigid3dTypes, Rigid3dMass>::reinit();
template<>
void UniformMass<Rigid3dTypes, Rigid3dMass>::loadRigidMass(std::string);
template <>
void UniformMass<Rigid3dTypes, Rigid3dMass>::draw();
template <>
void UniformMass<Rigid2dTypes, Rigid2dMass>::draw();
template <>
double UniformMass<Rigid3dTypes,Rigid3dMass>::getPotentialEnergy( const VecCoord& x );
template <>
double UniformMass<Rigid2dTypes,Rigid2dMass>::getPotentialEnergy( const VecCoord& x );
template <>
void UniformMass<Vec6dTypes,double>::draw();
#endif
#ifndef SOFA_DOUBLE
template<>
void UniformMass<Rigid3fTypes, Rigid3fMass>::reinit();
template<>
void UniformMass<Rigid3fTypes, Rigid3fMass>::loadRigidMass(std::string);
template <>
void UniformMass<Rigid3fTypes, Rigid3fMass>::draw();
template <>
void UniformMass<Rigid2fTypes, Rigid2fMass>::draw();
template <>
double UniformMass<Rigid3fTypes,Rigid3fMass>::getPotentialEnergy( const VecCoord& x );
template <>
double UniformMass<Rigid2fTypes,Rigid2fMass>::getPotentialEnergy( const VecCoord& x );
template <>
void UniformMass<Vec6fTypes,float>::draw();
#endif



} // namespace mass

} // namespace component

} // namespace sofa

#endif
