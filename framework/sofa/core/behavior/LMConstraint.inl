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
#ifndef SOFA_CORE_BEHAVIOR_LMCONSTRAINT_INL
#define SOFA_CORE_BEHAVIOR_LMCONSTRAINT_INL

#include <sofa/core/behavior/LMConstraint.h>
#include <sofa/core/behavior/BaseMechanicalMapping.h>
#include <sofa/core/objectmodel/BaseNode.h>

namespace sofa
{

namespace core
{
namespace behavior
{

template<class DataTypes1,class DataTypes2>
LMConstraint<DataTypes1,DataTypes2>::~LMConstraint()
{
}

template<class DataTypes1,class DataTypes2>
void   LMConstraint<DataTypes1,DataTypes2>::registerEquationInJ1(unsigned int constraintId, const SparseVecDeriv1 &C1) const
{
    //VecConst interface:
    //index where the direction will be found
    constrainedObject1->getC()->push_back(C1);
    constrainedObject1->setConstraintId(constraintId);
}

template<class DataTypes1,class DataTypes2>
void   LMConstraint<DataTypes1,DataTypes2>::registerEquationInJ2(unsigned int constraintId, const SparseVecDeriv2 &C2) const
{
    //VecConst interface:
    //index where the direction will be found
    constrainedObject2->getC()->push_back(C2);
    constrainedObject2->setConstraintId(constraintId);
}



template<class DataTypes1,class DataTypes2>
void LMConstraint<DataTypes1,DataTypes2>::init()
{
    BaseLMConstraint::init();


    if (constrainedObject1 != NULL && constrainedObject2 != NULL)
    {
        //Constraint created by passing Mechanical State directly, need to find the name of the path to be able to save the scene eventually

        if (constrainedObject1->getContext() != getContext())
        {
            sofa::core::objectmodel::BaseContext *context = NULL;
            sofa::core::objectmodel::BaseNode*    currentNode = dynamic_cast< sofa::core::objectmodel::BaseNode *>(constrainedObject1->getContext());

            std::string constrainedObject_name=currentNode->getPathName();
            if (context != NULL) this->pathObject1.setValue(constrainedObject_name);
        }


        if (constrainedObject2->getContext() != getContext())
        {
            sofa::core::objectmodel::BaseContext *context = NULL;
            sofa::core::objectmodel::BaseNode*    currentNode = dynamic_cast< sofa::core::objectmodel::BaseNode *>(constrainedObject2->getContext());

            std::string constrainedObject_name=currentNode->getPathName();
            if (context != NULL) this->pathObject2.setValue(constrainedObject_name);
        }

        simulatedObject1=constrainedObject1;
        while (simulatedObject1)
        {
            core::behavior::BaseMechanicalMapping* mapping;
            simulatedObject1->getContext()->get(mapping);
            if (!mapping) break;
            simulatedObject1 = mapping->getMechFrom()[0];
        }

        simulatedObject2=constrainedObject2;
        while (simulatedObject2)
        {
            core::behavior::BaseMechanicalMapping* mapping;
            simulatedObject2->getContext()->get(mapping);
            if (!mapping) break;
            simulatedObject2 = mapping->getMechFrom()[0];
        }
    }
}


} // namespace behavior

} // namespace core

} // namespace sofa

#endif
