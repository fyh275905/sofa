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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_CONSTRAINT_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_CONSTRAINT_INL

#include <sofa/core/componentmodel/behavior/Constraint.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template<class DataTypes>
Constraint<DataTypes>::Constraint(MechanicalState<DataTypes> *mm)
    : endTime( initData(&endTime,(Real)-1,"endTime","The constraint stops acting after the given value. Une a negative value for infinite constraints") )
    , mstate(mm)
{
}

template<class DataTypes>
Constraint<DataTypes>::~Constraint()
{
}

template <class DataTypes>
bool Constraint<DataTypes>::isActive() const
{
    if( endTime.getValue()<0 ) return true;
    return endTime.getValue()>getContext()->getTime();
}

template<class DataTypes>
void Constraint<DataTypes>::init()
{
    BaseConstraint::init();
    mstate = dynamic_cast< MechanicalState<DataTypes>* >(getContext()->getMechanicalState());
}

template<class DataTypes>
void Constraint<DataTypes>::projectResponse()
{
    if( !isActive() ) return;
    if (mstate)
        projectResponse(*mstate->getDx());
}

template<class DataTypes>
void Constraint<DataTypes>::projectVelocity()
{
    if( !isActive() ) return;
    if (mstate)
        projectVelocity(*mstate->getV());
}

template<class DataTypes>
void Constraint<DataTypes>::projectPosition()
{
    if( !isActive() ) return;
    if (mstate)
        projectPosition(*mstate->getX());
}

template<class DataTypes>
void Constraint<DataTypes>::projectFreeVelocity()
{
    if( !isActive() ) return;
    if (mstate)
        projectVelocity(*mstate->getVfree());
}

template<class DataTypes>
void Constraint<DataTypes>::projectFreePosition()
{
    if( !isActive() ) return;
    if (mstate)
        projectPosition(*mstate->getXfree());
}

template<class DataTypes>
void Constraint<DataTypes>::applyConstraint(unsigned int &contactId)
{
    if( !isActive() ) return;
    if (mstate)
        applyConstraint(*mstate->getC(), contactId);
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
