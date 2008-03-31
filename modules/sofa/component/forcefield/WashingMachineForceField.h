/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
//
// C++ Interface: WashingMachineForceField
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_COMPONENT_FORCEFIELD_WASHINGMACHINEFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_WASHINGMACHINEFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/component/forcefield/PlaneForceField.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class WashingMachineForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef PlaneForceField<DataTypes> PlaneForceFieldT;

protected:
    core::objectmodel::Data<Coord> _center;
    core::objectmodel::Data<Deriv> _size;
    core::objectmodel::Data<Real> _speed;

    core::objectmodel::Data<Real> _stiffness;
    core::objectmodel::Data<Real> _damping;

    defaulttype::Vec<6,PlaneForceFieldT*> _planes;

public:
    WashingMachineForceField(core::componentmodel::behavior::MechanicalState<DataTypes>* object=NULL, const std::string& /*name*/="")
        : core::componentmodel::behavior::ForceField<DataTypes>(object)
        , _center(initData(&_center, Coord(0,0,0), "center", "box center"))
        , _size(initData(&_size, Deriv(1,1,1), "size", "box size"))
        , _speed(initData(&_speed, (Real)0.01, "speed", "rotation speed"))
        , _stiffness(initData(&_stiffness, (Real)500.0, "stiffness", "penality force stiffness"))
        , _damping(initData(&_damping, (Real)5.0, "damping", "penality force damping"))
    {
    }


    ~WashingMachineForceField()
    {
        for(int i=0; i<6; ++i)
            delete _planes[i];
    }


    virtual void init()
    {
        for(int i=0; i<6; ++i)
        {
            _planes[i] = new PlaneForceFieldT;
            _planes[i]->setContext(this->getContext());
            _planes[i]->setStiffness(_stiffness.getValue());
            _planes[i]->setDamping(_damping.getValue());
        }

        Deriv diff = _center.getValue() - _size.getValue() * .5;
        Deriv diff2 = - _center.getValue() - _size.getValue() * .5;

        _planes[0]->setPlane( Deriv( 0, 1, 0), diff[1]  ); // sud
        _planes[1]->setPlane( Deriv( 0, -1, 0), diff2[1]  ); // nord
        _planes[2]->setPlane( Deriv( -1, 0, 0), diff2[0]  ); // ouest
        _planes[3]->setPlane( Deriv( 1, 0, 0), diff[0]  ); // est
        _planes[4]->setPlane( Deriv( 0, 0, 1), diff[2]  ); // derriere
        _planes[5]->setPlane( Deriv( 0, 0, -1), diff2[2]  ); //devant

        _planes[0]->color.setValue( Coord( 0.5f,0.4f,0.4f ) );
        _planes[1]->color.setValue( Coord( 0.4f,0.5f,0.4f ) );
        _planes[2]->color.setValue( Coord( 0.4f,0.4f,0.5f ) );
        _planes[3]->color.setValue( Coord( 0.5f,0.5f,0.4f ) );
        _planes[4]->color.setValue( Coord( 0.5f,0.4f,0.5f ) );
        _planes[5]->color.setValue( Coord( 0.4f,0.5f,0.5f ) );

    }

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    virtual double getPotentialEnergy(const VecCoord& x);


    void draw();
    bool addBBox(double* minBBox, double* maxBBox);
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
