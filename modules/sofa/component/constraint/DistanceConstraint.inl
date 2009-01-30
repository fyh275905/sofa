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
#ifndef SOFA_COMPONENT_CONSTRAINT_DISTANCECONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_DISTANCECONSTRAINT_INL

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/componentmodel/behavior/BaseLMConstraint.h>
#include <sofa/component/constraint/DistanceConstraint.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/helper/vector.h>
#include <sofa/helper/gl/template.h>
#include <iostream>





namespace sofa
{

namespace component
{

namespace constraint
{

using namespace core::componentmodel::topology;

using namespace sofa::defaulttype;
using namespace sofa::helper;
using namespace sofa::core::componentmodel::behavior;



template <class DataTypes>
void DistanceConstraint<DataTypes>::init()
{
    LMConstraint<DataTypes,DataTypes>::init();
    topology = this->getContext()->getMeshTopology();
    if (vecConstraint.getValue().size() == 0 && (this->object1==this->object2) ) vecConstraint.setValue(this->topology->getEdges());
}

template <class DataTypes>
void DistanceConstraint<DataTypes>::reinit()
{
    updateRestLength();
}



template <class DataTypes>
double DistanceConstraint<DataTypes>::lengthEdge(const Edge &e, const VecCoord &x1, const VecCoord &x2)
{
    return (x2[e[1]] -  x1[e[0]]).norm();
}



template <class DataTypes>
void DistanceConstraint<DataTypes>::updateRestLength()
{
    const VecCoord &x0_1=*this->object1->getX();
    const VecCoord &x0_2=*this->object2->getX();
    const SeqEdges &edges =  vecConstraint.getValue();
    this->l0.resize(edges.size());
    for (unsigned int i=0; i<edges.size(); ++i)
    {
        this->l0[i] = lengthEdge(edges[i],x0_1,x0_2);
    }
}

#ifndef SOFA_FLOAT
template<>
Rigid3dTypes::Deriv DistanceConstraint<Rigid3dTypes>::getDirection(const Edge &e, const VecCoord &x1, const VecCoord &x2);
#endif
#ifndef SOFA_DOUBLE
template<>
Rigid3fTypes::Deriv DistanceConstraint<Rigid3fTypes>::getDirection(const Edge &e, const VecCoord &x1, const VecCoord &x2);
#endif

template<class DataTypes>
typename DataTypes::Deriv DistanceConstraint<DataTypes>::getDirection(const Edge &e, const VecCoord &x1, const VecCoord &x2)
{
    Deriv V12 = (x2[e[1]] - x1[e[0]]);
    V12.normalize();
    return V12;
}


template<class DataTypes>
void DistanceConstraint<DataTypes>::writeConstraintEquations()
{
    const VecCoord &x1=*(this->object1->getX());
    const VecCoord &x2=*(this->object2->getX());
    VecConst& c1 = *this->object1->getC();
    VecConst& c2 = *this->object2->getC();
    this->clear();

    const SeqEdges &edges =  vecConstraint.getValue();

    if (this->l0.size() != edges.size()) updateRestLength();


    for (unsigned int i=0; i<edges.size(); ++i)
    {
        unsigned int idx1=edges[i][0];
        unsigned int idx2=edges[i][1];

        double length     = lengthEdge(edges[i],x1,x2);
        double restLength = this->l0[i];

        Deriv V12 = getDirection(edges[i], x1, x2);
        //VecConst interface:
        //index where the direction will be found
        const unsigned int idxInVecConst[2]= {c1.size(), c2.size()};
        SparseVecDeriv V1;
        V1.insert(idx1,V12); c1.push_back(V1);

        if (this->object1 != this->object2)
        {
            SparseVecDeriv V2;
            V2.insert(idx2,V12); c2.push_back(V2);
        }

        core::componentmodel::behavior::BaseLMConstraint::constraintGroup *constraintAcc = addGroupConstraint(this->ACC);
        core::componentmodel::behavior::BaseLMConstraint::constraintGroup *constraintVel = addGroupConstraint(this->VEL);
        core::componentmodel::behavior::BaseLMConstraint::constraintGroup *constraintPos = addGroupConstraint(this->POS);

        //BaseLMConstraint interface
        constraintAcc->addConstraint(idxInVecConst[0], idxInVecConst[1],
                0, this->FINAL); //0 in acceleration along V12
        constraintVel->addConstraint( idxInVecConst[0], idxInVecConst[1],
                0, this->FINAL); //0 in velocity along V12
        constraintPos->addConstraint( idxInVecConst[0], idxInVecConst[1],
                restLength-length, this->CORRECTION); //we apply a constraint to correct the current length to the rest length

    }
}

template <class DataTypes>
double DistanceConstraint<DataTypes>::getError()
{
    double error=0.0;
    const VecCoord &x1=*(this->object1->getX());
    const VecCoord &x2=*(this->object2->getX());


    const SeqEdges &edges = vecConstraint.getValue();
    for (unsigned int i=0; i<edges.size(); ++i)
    {
        double length     = lengthEdge(edges[i],x1,x2);
        double restLength = this->l0[i];//lengthEdge(edges[i],x0);
        double deformation = fabs(length - restLength);
        {
            error += pow(deformation,2);
        }
    }
    return error;
}

#ifndef SOFA_FLOAT
template <>
void DistanceConstraint<defaulttype::Rigid3dTypes>::draw();
#endif
#ifndef SOFA_DOUBLE
template <>
void DistanceConstraint<defaulttype::Rigid3fTypes>::draw();
#endif

template <class DataTypes>
void DistanceConstraint<DataTypes>::draw()
{
    if (this->l0.size() != vecConstraint.getValue().size()) updateRestLength();

    sout << getError() << " Error" << sendl;
    if (this->getContext()->getShowBehaviorModels())
    {
        const VecCoord &x1=*(this->object1->getX());
        const VecCoord &x2=*(this->object2->getX());

        std::vector< Vector3 > points;
        const SeqEdges &edges =  vecConstraint.getValue();
        for (unsigned int i=0; i<edges.size(); ++i)
        {
//                 double length     = lengthEdge(edges[i],x1,x2);
//                 double restLength = this->l0[i];
//                 double factor = fabs(length - restLength)/length;
            points.push_back(x1[edges[i][0]]);
            points.push_back(x2[edges[i][1]]);
        }
        simulation::tree::getSimulation()->DrawUtility.drawLines(points, 1, Vec<4,float>(0.0,1.0,0.0f,1.0f));
    }
}




} // namespace constraint

} // namespace component

} // namespace sofa

#endif


