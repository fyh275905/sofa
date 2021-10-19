/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <sofa/core/behavior/MixedInteractionForceField.h>
#include <sofa/core/MechanicalParams.h>

namespace sofa::core::behavior
{

template<class DataTypes1, class DataTypes2>
MixedInteractionForceField<DataTypes1, DataTypes2>::MixedInteractionForceField(MechanicalState<DataTypes1> *mm1, MechanicalState<DataTypes2> *mm2)
    : Inherit1(), Inherit2(mm1, mm2)
{
    if (!mm1)
        mstate1.setPath("@./"); // default to state of the current node
    if (!mm2)
        mstate2.setPath("@./"); // default to state of the current node
}

template<class DataTypes1, class DataTypes2>
MixedInteractionForceField<DataTypes1, DataTypes2>::~MixedInteractionForceField()
{
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addForce(const MechanicalParams* mparams, MultiVecDerivId fId )
{

    if (mstate1 && mstate2)
    {

        addForce( mparams, *fId[mstate1.get()].write(), *fId[mstate2.get()].write(),
                 *mparams->readX(mstate1), *mparams->readX(mstate2),
                 *mparams->readV(mstate1), *mparams->readV(mstate2) );

    }
}

template<class DataTypes1, class DataTypes2>
void MixedInteractionForceField<DataTypes1, DataTypes2>::addDForce(const MechanicalParams* mparams, MultiVecDerivId dfId )
{
    if (mstate1 && mstate2)
    {
            addDForce( mparams, *dfId[mstate1.get()].write()    , *dfId[mstate2.get()].write()   ,
                    *mparams->readDx(mstate1) , *mparams->readDx(mstate2) );
    }
}



template<class DataTypes1, class DataTypes2>
SReal MixedInteractionForceField<DataTypes1, DataTypes2>::getPotentialEnergy(const MechanicalParams* mparams) const
{
    if (mstate1 && mstate2)
        return getPotentialEnergy(mparams, *mparams->readX(mstate1),*mparams->readX(mstate2));
    else return 0;
}

} // namespace sofa::core::behavior

