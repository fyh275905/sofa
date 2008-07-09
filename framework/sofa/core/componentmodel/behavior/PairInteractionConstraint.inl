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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_PAIRINTERACTIONCONSTRAINT_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_PAIRINTERACTIONCONSTRAINT_INL

#include <sofa/core/objectmodel/DataPtr.h>
#include "PairInteractionConstraint.h"

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template<class DataTypes>
PairInteractionConstraint<DataTypes>::PairInteractionConstraint(MechanicalState<DataTypes> *mm1, MechanicalState<DataTypes> *mm2)
    : endTime( initData(&endTime,(double)-1,"endTime","The constraint stops acting after the given value.\nUse a negative value for infinite constraints") )
    , mstate1(mm1), mstate2(mm2)
{
}

template<class DataTypes>
PairInteractionConstraint<DataTypes>::~PairInteractionConstraint()
{
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::init()
{
    InteractionConstraint::init();
    if (mstate1 == NULL || mstate2 == NULL)
    {
        mstate1 = mstate2 = dynamic_cast< MechanicalState<DataTypes>* >(getContext()->getMechanicalState());
    }
}

template<class DataTypes>
bool PairInteractionConstraint<DataTypes>::isActive() const
{
    if( endTime.getValue()<0 ) return true;
    return endTime.getValue()>getContext()->getTime();
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::projectResponse()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        projectResponse(*mstate1->getDx(), *mstate2->getDx());
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::projectVelocity()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        projectVelocity(*mstate1->getV(), *mstate2->getV());
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::projectPosition()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        projectPosition(*mstate1->getX(), *mstate2->getX());
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::projectFreeVelocity()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        projectVelocity(*mstate1->getVfree(), *mstate2->getVfree());
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::projectFreePosition()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        projectPosition(*mstate1->getXfree(), *mstate2->getXfree());
}

template<class DataTypes>
void PairInteractionConstraint<DataTypes>::applyConstraint(unsigned int &contactId)
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
        applyConstraint(*mstate1->getC(), *mstate2->getC(), contactId);
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
