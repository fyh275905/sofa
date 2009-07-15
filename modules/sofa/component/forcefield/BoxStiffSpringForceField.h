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
#ifndef SOFA_COMPONENT_CONSTRAINT_BOXSTIFFSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_CONSTRAINT_BOXSTIFFSPRINGFORCEFIELD_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <sofa/defaulttype/Vec.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

/** Set springs between the particles located inside a given box.
*/
template <class DataTypes>
class BoxStiffSpringForceField : public StiffSpringForceField<DataTypes>
{
public:
    typedef StiffSpringForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef typename Inherit::Spring Spring;
    typedef core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalState;
    enum { N=Coord::static_size };

    typedef defaulttype::Vec<6,Real> Vec6;

protected:

    //float Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;

public:

    BoxStiffSpringForceField(MechanicalState* object1, MechanicalState* object2, double ks=100.0, double kd=5.0);
    BoxStiffSpringForceField(double ks=100.0, double kd=5.0);

    void bwdInit();

    //virtual const char* getTypeName() const { return "Boxstiffspringforcefield"; }
    Data<Vec6>  box_object1;
    Data<Vec6>  box_object2;
    Data<SReal> factorRestLength;
    // -- VisualModel interface

    void draw();

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
