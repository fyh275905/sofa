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
//
// C++ Interface: ConicalForceField
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_COMPONENT_FORCEFIELD_CONICALFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_CONICALFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/Data.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class ConicalForceFieldInternalData
{
public:
};

template<class DataTypes>
class ConicalForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

protected:
    class Contact
    {
    public:
        int index;
        Coord normal;
        Coord pos;
        Contact( int index=0, Coord normal=Coord(),Coord pos=Coord())
            : index(index),normal(normal),pos(pos)
        {
        }

        inline friend std::istream& operator >> ( std::istream& in, Contact& c )
        {
            in>>c.index>>c.normal>>c.pos;
            return in;
        }

        inline friend std::ostream& operator << ( std::ostream& out, const Contact& c )
        {
            out << c.index << " " << c.normal << " " << c.pos ;
            return out;
        }

    };

    Data<sofa::helper::vector<Contact> > contacts;

    ConicalForceFieldInternalData<DataTypes> data;

public:

    Data<Coord> coneCenter;
    Data<Coord> coneHeight;
    Data<Real> coneAngle;

    Data<Real> stiffness;
    Data<Real> damping;
    Data<defaulttype::Vec3f> color;
    Data<bool> bDraw;

    ConicalForceField()
        : coneCenter(initData(&coneCenter, "coneCenter", "cone center"))
        , coneHeight(initData(&coneHeight, "coneHeight", "cone height"))
        , coneAngle(initData(&coneAngle, (Real)10, "coneAngle", "cone angle"))

        , stiffness(initData(&stiffness, (Real)500, "stiffness", "force stiffness"))
        , damping(initData(&damping, (Real)5, "damping", "force damping"))
        , color(initData(&color, defaulttype::Vec3f(0.0f,0.0f,1.0f), "color", "cone color"))
        , bDraw(initData(&bDraw, true, "draw", "enable/disable drawing of the cone"))
    {
    }

    void setCone(const Coord& center, Coord height, Real angle)
    {
        coneCenter.setValue( center );
        coneHeight.setValue( height );
        coneAngle.setValue( angle );
    }

    void setStiffness(Real stiff)
    {
        stiffness.setValue( stiff );
    }

    void setDamping(Real damp)
    {
        damping.setValue( damp );
    }

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    virtual double getPotentialEnergy(const VecCoord& x);

    virtual void updateStiffness( const VecCoord& x );

    virtual bool isIn(Coord p);

    void draw();
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
