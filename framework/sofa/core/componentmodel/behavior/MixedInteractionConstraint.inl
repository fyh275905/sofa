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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONCONSTRAINT_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MIXEDINTERACTIONCONSTRAINT_INL

#include "MixedInteractionConstraint.h"
#include <sofa/core/componentmodel/behavior/Constraint.inl>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template<class DataTypes1, class DataTypes2>
MixedInteractionConstraint<DataTypes1, DataTypes2>::MixedInteractionConstraint(MechanicalState<DataTypes1> *mm1, MechanicalState<DataTypes2> *mm2)
    : endTime( initData(&endTime,(double)-1,"endTime","The constraint stops acting after the given value.\nUse a negative value for infinite constraints") )
    , object1( initData(&object1, "object1", "First Object to Constraint"))
    , object2( initData(&object2, "object2", "Second Object to Constraint"))
    , mstate1(mm1), mstate2(mm2)
{
}

template<class DataTypes1, class DataTypes2>
MixedInteractionConstraint<DataTypes1, DataTypes2>::~MixedInteractionConstraint()
{
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::init()
{
    InteractionConstraint::init();
    this->mask1 = &mstate1->forceMask;
    this->mask2 = &mstate2->forceMask;
}

template<class DataTypes1, class DataTypes2>
bool MixedInteractionConstraint<DataTypes1, DataTypes2>::isActive() const
{
    if( endTime.getValue()<0 ) return true;
    return endTime.getValue()>getContext()->getTime();
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::projectResponse()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        mstate1->forceMask.setInUse(this->useMask());
        mstate2->forceMask.setInUse(this->useMask());
        projectResponse(*mstate1->getDx(), *mstate2->getDx());
    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::projectVelocity()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        this->mask1 = &mstate1->forceMask;
        this->mask2 = &mstate2->forceMask;
        projectVelocity(*mstate1->getV(), *mstate2->getV());
    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::projectPosition()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        this->mask1 = &mstate1->forceMask;
        this->mask2 = &mstate2->forceMask;
        projectPosition(*mstate1->getX(), *mstate2->getX());
    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::projectFreeVelocity()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        this->mask1 = &mstate1->forceMask;
        this->mask2 = &mstate2->forceMask;
        projectVelocity(*mstate1->getVfree(), *mstate2->getVfree());
    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::projectFreePosition()
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        this->mask1 = &mstate1->forceMask;
        this->mask2 = &mstate2->forceMask;
        projectPosition(*mstate1->getXfree(), *mstate2->getXfree());
    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionConstraint<DataTypes1, DataTypes2>::applyConstraint(unsigned int &contactId)
{
    if( !isActive() ) return;
    if (mstate1 && mstate2)
    {
        this->mask1 = &mstate1->forceMask;
        this->mask2 = &mstate2->forceMask;
        applyConstraint(*mstate1->getC(), *mstate2->getC(), contactId);
    }
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
