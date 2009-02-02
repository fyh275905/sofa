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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_LAGRANGEMULTIPLIERINTERACTION_INL
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_LAGRANGEMULTIPLIERINTERACTION_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include "LagrangeMultiplierInteraction.h"
#include <sofa/helper/gl/template.h>
#include <sofa/helper/system/config.h>
#include <sofa/helper/system/glut.h>
#include <sofa/helper/rmath.h>
#include <assert.h>
#include <iostream>

#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/ObjectFactory.h>
/*#include <sofa/core/componentmodel/collision/DetectionOutput.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>
#include <sofa/core/componentmodel/topology/TopologicalMapping.h>
#include <sofa/helper/gl/template.h>
*/

#include <sofa/simulation/common/BehaviorUpdatePositionVisitor.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/FindByTypeVisitor.h>
#include <sofa/simulation/common/Node.h>

#include <sofa/component/linearsolver/FullVector.h>
using namespace sofa::component::linearsolver;

namespace sofa
{

namespace component
{

namespace interactionforcefield
{


template<class DataTypes1, class DataTypes2>
void LagrangeMultiplierInteraction<DataTypes1, DataTypes2>::init()
{
    core::componentmodel::behavior::InteractionForceField::init();

    core::objectmodel::BaseContext* test = this->getContext();

    // ancien mécanisme: une seule contrainte
    test->get(constraint,f_constraint.getValue());


    // nouveau mécanisme: plusieurs contraintes
    simulation::FindByTypeVisitor< core::componentmodel::behavior::BaseConstraint > findConstraint;
    findConstraint.execute(getContext());
    list_base_constraint = findConstraint.found;


    // debug //
    //sout<<"************** list of constraints : *************"<<sendl;
    long id[100];
    unsigned int offset;
    for (unsigned int i=0; i<list_base_constraint.size(); i++)
    {
        sout<<list_base_constraint[i]->getName()<<sendl;

        baseConstraint* bc =list_base_constraint[i];

        SimpleConstraint * sc = dynamic_cast<SimpleConstraint *>(bc);
        if (sc != NULL)
        {
            //debug
            //serr<< "simple constraint applied on object "<<sc->getMState()->getName() <<sendl;
            list_constraint.push_back(sc);
        }

        InteractionConstraint* ic= dynamic_cast<InteractionConstraint*> (bc);
        if (ic != NULL)
        {
            // debug
            //serr<< "interaction constraint applied on object "<<ic->getMechModel1()->getName() <<"  and on object "<< ic->getMechModel2()->getName()<<sendl;
            //serr<< "mstate1 : "<<this->mstate2->getName()<<sendl;

            core::objectmodel::BaseContext* context_model1 = ic->getMechModel1()->getContext();
            core::objectmodel::BaseContext* context_model2 = ic->getMechModel2()->getContext();
            if (this->mstate2==context_model1->getMechanicalState() || this->mstate2==context_model2->getMechanicalState() )
            {
                sout<<" - this constraint must be handled - "<<sendl;
                list_interaction_constraint.push_back(ic);


                offset=0;
                ic->getConstraintId(id, offset);
                sout<< "constraint offset"<<offset<<sendl;
            }
        }
    }

    for (unsigned int i=0; i<offset; i++)
    {
        sout<< "id : "<< id[i] <<sendl;
    }
}


template<class DataTypes1, class DataTypes2>
void LagrangeMultiplierInteraction<DataTypes1, DataTypes2>::addForce(VecDeriv1& violation, VecDeriv2& ,
        const VecCoord1& , const VecCoord2& ,
        const VecDeriv1& , const VecDeriv2& )
{

    unsigned int count=0;

    for (unsigned int i=0; i<list_interaction_constraint.size(); i++)
    {
        list_interaction_constraint[i]->applyConstraint(count);
        sout<< "constraint count"<<count<<sendl;
    }
    unsigned int count1=0;
    constraint->applyConstraint(count1);


    /// @TODO clear the MechanicalState of the lagrange Multiplier during Begin visitor
    // clear the mechanical state of lagrange Multiplier
    // should be done elsewhere //
    VecCoord1& lambda= *this->mstate1->getX();
    unsigned int numLagMult = lambda.size();
    lambda.clear();
    lambda.resize(numLagMult);
    // clear the Velocity ? useful ?
    VecDeriv1& Dlambda= *this->mstate1->getV();
    Dlambda.clear();
    Dlambda.resize(numLagMult);

    FullVector<double> _violation;

    /// @TODO automatically adapt the size of the LagMult state to the num of constraints: for now it is based the scene file entries
    _violation.resize(numLagMult);

    constraint->getConstraintValue(&_violation, false);
    //sout<<"violation:" <<_violation[0] << " "<<_violation[1] << " "<<_violation[2] << " "<<sendl;

    for (unsigned int i=0; i<lambda.size(); i++)
    {
        violation[i].x() = _violation[i];
    }

}
/*
template<class DataTypes1, class DataTypes2>
void LagranintgeMultiplierInteraction<DataTypes1, DataTypes2>::addForce2(VecDeriv1& f1, VecDeriv2& f2,
																	  const VecCoord1& p1, const VecCoord2& p2,
																	  const VecDeriv1& v1, const VecDeriv2& v2)
{



}
*/

template<class DataTypes1, class DataTypes2>
void LagrangeMultiplierInteraction<DataTypes1, DataTypes2>::addDForce(VecDeriv1& dViolation, VecDeriv2& df2, const VecDeriv1& dLambda, const VecDeriv2& dx2)
{

    //sout<<"addDForce : dLambda "<< dLambda << " -  dx2:" << dx2 <<sendl;


    sofa::simulation::Node *context = dynamic_cast<sofa::simulation::Node *>(this->getContext()); // access to current node (which is supposed to be the root)
    sofa::simulation::MechanicalResetConstraintVisitor().execute(context);

    VecConst2& c2= *this->mstate2->getC();
    //sout<<" constraint size :"<<c2.size()<<sendl;
    typename std::map< unsigned int, Deriv2>::const_iterator it;


    for (unsigned int i=0; i< c2.size(); i++)
    {
        SparseVecDeriv2 constraint = c2[i];

        //sout<<" i= "<<i <<"   constraint size= "<< constraint.size() <<sendl;
        for (it=constraint.getData().begin(); it!=constraint.getData().end(); it++)
        {
            //sout<<" constraint : i "<< constraint[j].index  << "  data"<< constraint[j].data << sendl;
            /// @TODO : use the constraint ID
            //Deriv2 dV0 = constraint[j].data * dx2[constraint[j].index];
            unsigned int  index=it->first;
            Deriv2 value=it->second;
            dViolation[i].x() += value * dx2[index];
            df2[index] += value * dLambda[i].x();

        }
    }


    //sout<<"addDForce : dViolation "<< dViolation << " -  df2:" << df2 <<sendl;


}

/*
template <class DataTypes1, class DataTypes2>
    double LagrangeMultiplierInteraction<DataTypes1, DataTypes2>::getPotentialEnergy(const VecCoord1& x1 , const VecCoord2& x2 )
{
    serr<<"LagrangeMultiplierInteraction::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}
*/

/*
template<class DataTypes1, class DataTypes2>
void LagrangeMultiplierInteraction<DataTypes1, DataTypes2>::draw()
{


}
*/

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
