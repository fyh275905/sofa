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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_FORCEFIELD_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_FORCEFIELD_INL

#include <sofa/core/objectmodel/DataPtr.h>
#include "ForceField.h"

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template<class DataTypes>
ForceField<DataTypes>::ForceField(MechanicalState<DataTypes> *mm)
    : mstate(mm)
{
}

template<class DataTypes>
ForceField<DataTypes>::~ForceField()
{
}

template<class DataTypes>
void ForceField<DataTypes>::init()
{
    BaseForceField::init();
    mstate = dynamic_cast< MechanicalState<DataTypes>* >(getContext()->getMechanicalState());
}

template<class DataTypes>
void ForceField<DataTypes>::addForce()
{
    if (mstate)
        addForce(*mstate->getF(), *mstate->getX(), *mstate->getV());
}

template<class DataTypes>
void ForceField<DataTypes>::addDForce(double kFactor, double bFactor)
{
    if (mstate)
        addDForce(*mstate->getF(), *mstate->getDx(), kFactor, bFactor);
}

template<class DataTypes>
void ForceField<DataTypes>::addDForceV(double kFactor, double bFactor)
{
    if (mstate)
        addDForce(*mstate->getF(), *mstate->getV(), kFactor, bFactor);
}
template<class DataTypes>
void ForceField<DataTypes>::addDForce(VecDeriv& /*df*/, const VecDeriv& /*dx*/)
{
    std::cerr << "ERROR("<<getClassName()<<"): addDForce not implemented.\n";
}

template<class DataTypes>
void ForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx, double kFactor, double /*bFactor*/)
{
    if (kFactor == 1.0)
        addDForce(df, dx);
    else if (kFactor != 0.0)
    {
        BaseMechanicalState::VecId vtmp(BaseMechanicalState::VecId::V_DERIV,BaseMechanicalState::VecId::V_FIRST_DYNAMIC_INDEX);
        mstate->vAvail(vtmp);
        mstate->vAlloc(vtmp);
        BaseMechanicalState::VecId vdx(BaseMechanicalState::VecId::V_DERIV,0);
        /// @TODO: Add a better way to get the current VecId of dx
        for (vdx.index=0; vdx.index<vtmp.index; ++vdx.index)
            if (mstate->getVecDeriv(vdx.index) == &dx)
                break;
        mstate->vOp(vtmp,BaseMechanicalState::VecId::null(),vdx,kFactor);
        addDForce(df, *mstate->getVecDeriv(vtmp.index));
        mstate->vFree(vtmp);
    }
}

template<class DataTypes>
double ForceField<DataTypes>::getPotentialEnergy()
{
    if (mstate)
        return getPotentialEnergy(*mstate->getX());
    else return 0;
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
