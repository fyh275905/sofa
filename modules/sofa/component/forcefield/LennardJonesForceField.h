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
#ifndef SOFA_COMPONENT_FORCEFIELD_LENNARDJONESFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_LENNARDJONESFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class LennardJonesForceField : public sofa::core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef sofa::core::componentmodel::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

protected:
    Real a,b;
    Data<Real> alpha,beta,dmax,fmax;
    Data<Real> d0,p0;
    Data<Real> damping;

    struct DForce
    {
        unsigned int a,b;
        Real df;
    };

    sofa::helper::vector<DForce> dforces;

public:
    LennardJonesForceField()
        : a(1)
        , b(1)
        , alpha  (initData(&alpha  ,Real(6), "alpha"  ,"Alpha"))
        , beta   (initData(&beta   ,Real(12),"beta"   ,"Beta"))
        , dmax   (initData(&dmax   ,Real(2), "dmax"   ,"DMax"))
        , fmax   (initData(&fmax   ,Real(1), "fmax"   ,"FMax"))
        , d0     (initData(&d0     ,Real(1), "d0"     ,"d0"))
        , p0     (initData(&p0     ,Real(1), "p0"     ,"p0"))
        , damping(initData(&damping,Real(0), "damping","Damping"))
    {
    }

    void setAlpha(Real v) { alpha.setValue(v); }
    void setBeta(Real v) { beta.setValue(v); }
    void setFMax(Real v) { fmax.setValue(v); }
    void setDMax(Real v) { dmax.setValue(v); }
    void setD0(Real v) { d0.setValue(v); }
    void setP0(Real v) { p0.setValue(v); }
    void setDamping(Real v) { damping.setValue(v); }


    virtual void init();

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    virtual double getPotentialEnergy(const VecCoord& x);

    void draw();

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
