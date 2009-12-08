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
#ifndef SOFA_COMPONENT_MASS_FRAMEMASS_H
#define SOFA_COMPONENT_MASS_FRAMEMASS_H

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/componentmodel/behavior/Mass.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
//#include <sofa/component/contextobject/CoordinateSystem.h>
#include <sofa/defaulttype/BaseVector.h>
#include <sofa/core/objectmodel/DataFileName.h>

namespace sofa
{

namespace defaulttype
{

template<int N, typename Real>
class FrameMass;

template<typename real>
class FrameMass<3, real>
{
public:
    typedef real value_type;
    typedef real Real;
    typedef typename StdRigidTypes<3,Real>::VecCoord VecCoord;
    typedef typename StdRigidTypes<3,Real>::VecDeriv VecDeriv;
    typedef typename StdRigidTypes<3,Real>::Coord Coord;
    typedef typename StdRigidTypes<3,Real>::Deriv Deriv;
    typedef Mat<36,6,Real> Mat36;
    typedef Mat<6,6,Real> Mat66;
    typedef vector<double> VD;
    Real mass,volume;
    Mat66 inertiaMatrix;	      // Inertia matrix of the object
    Mat66 inertiaMassMatrix;    // Inertia matrix of the object * mass of the object
    Mat66 invInertiaMatrix;	  // inverse of inertiaMatrix
    Mat66 invInertiaMassMatrix; // inverse of inertiaMassMatrix

    FrameMass ( Real m=1 )
    {
        mass = m;
        volume = 1;
        recalc();
    }

    void operator= ( Real m )
    {
        mass = m;
        recalc();
    }
    // operator to cast to const Real
    operator const Real() const
    {
        return mass;
    }
    void recalc()
    {
        inertiaMassMatrix = inertiaMatrix * mass;
        invInertiaMatrix.invert ( inertiaMatrix );
        invInertiaMassMatrix.invert ( inertiaMassMatrix );
    }

    /// compute ma = M*a
    Deriv operator * ( const Deriv& a ) const
    {
        Vec6d va, vma;
        va[0] = a.getVCenter() [0];
        va[1] = a.getVCenter() [1];
        va[2] = a.getVCenter() [2];
        va[3] = a.getVOrientation() [0];
        va[4] = a.getVOrientation() [1];
        va[5] = a.getVOrientation() [2];

        vma = inertiaMassMatrix * va;

        Deriv ma;
        ma.getVCenter() [0] = vma[0];
        ma.getVCenter() [1] = vma[1];
        ma.getVCenter() [2] = vma[2];
        ma.getVOrientation() [0] = vma[3];
        ma.getVOrientation() [1] = vma[4];
        ma.getVOrientation() [2] = vma[5];

        return ma;
    }

    /// compute a = f/a
    Deriv operator / ( const Deriv& f ) const
    {
        Vec6d va, vma;
        vma[0] = f.getVCenter() [0];
        vma[1] = f.getVCenter() [1];
        vma[2] = f.getVCenter() [2];
        vma[3] = f.getVOrientation() [0];
        vma[4] = f.getVOrientation() [1];
        vma[5] = f.getVOrientation() [2];

        va = invInertiaMassMatrix * vma;

        Deriv a;
        a.getVCenter() [0] = va[0];
        a.getVCenter() [1] = va[1];
        a.getVCenter() [2] = va[2];
        a.getVOrientation() [0] = va[3];
        a.getVOrientation() [1] = va[4];
        a.getVOrientation() [2] = va[5];

        return a;
    }

    void operator *= ( Real fact )
    {
        mass *= fact;
        inertiaMassMatrix *= fact;
        invInertiaMassMatrix /= fact;
    }
    void operator /= ( Real fact )
    {
        mass /= fact;
        inertiaMassMatrix /= fact;
        invInertiaMassMatrix *= fact;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const FrameMass<3, real>& m )
    {
        out<<m.mass;
        out<<" "<<m.volume;
        out<<" "<<m.inertiaMatrix;
        return out;
    }
    inline friend std::istream& operator >> ( std::istream& in, FrameMass<3, real>& m )
    {
        in>>m.mass;
        in>>m.volume;
        in>>m.inertiaMatrix;
        return in;
    }

};

template<int N, typename real>
inline typename StdRigidTypes<N, real>::Deriv operator* ( const typename StdRigidTypes<N, real>::Deriv& d, const FrameMass<N,real>& m )
{
    return m * d;
}

template<int N, typename real>
inline typename StdRigidTypes<N, real>::Deriv operator/ ( const typename StdRigidTypes<N, real>::Deriv& d, const FrameMass<N, real>& m )
{
    return m / d;
}



typedef FrameMass<3,double> Frame3dMass;
typedef FrameMass<3,float> Frame3fMass;

} // namespace defaulttype

} // namespace sofa

#endif

