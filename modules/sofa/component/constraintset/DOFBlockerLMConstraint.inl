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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_CONSTRAINTSET_DOFBLOCKERLMCONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINTSET_DOFBLOCKERLMCONSTRAINT_INL

#include <sofa/component/constraintset/DOFBlockerLMConstraint.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/helper/gl/Axis.h>
#include <sofa/helper/gl/template.h>





namespace sofa
{

namespace component
{

namespace constraintset
{

using namespace sofa::helper;


// Define TestNewPointFunction
template< class DataTypes>
bool DOFBlockerLMConstraint<DataTypes>::FCTestNewPointFunction(int /*nbPoints*/, void* param, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >& )
{
    DOFBlockerLMConstraint<DataTypes> *fc= (DOFBlockerLMConstraint<DataTypes> *)param;
    if (fc)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Define RemovalFunction
template< class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::FCRemovalFunction(int pointIndex, void* param)
{
    DOFBlockerLMConstraint<DataTypes> *fc= (DOFBlockerLMConstraint<DataTypes> *)param;
    if (fc)
    {
        fc->removeConstraint((unsigned int) pointIndex);
    }
    return;
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::clearConstraints()
{
    f_indices.beginEdit()->clear();
    f_indices.endEdit();
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::addConstraint(unsigned int index)
{
    f_indices.beginEdit()->push_back(index);
    f_indices.endEdit();
}

template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::removeConstraint(unsigned int index)
{
    removeValue(*f_indices.beginEdit(),index);
    f_indices.endEdit();
}


template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::init()
{
    core::behavior::LMConstraint<DataTypes,DataTypes>::init();

    topology = this->getContext()->getMeshTopology();

    // Initialize functions and parameters
    topology::PointSubset my_subset = f_indices.getValue();

    my_subset.setTestFunction(FCTestNewPointFunction);
    my_subset.setRemovalFunction(FCRemovalFunction);

    my_subset.setTestParameter( (void *) this );
    my_subset.setRemovalParameter( (void *) this );
}

// Handle topological changes
template <class DataTypes> void DOFBlockerLMConstraint<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=topology->beginChange();
    std::list<const TopologyChange *>::const_iterator itEnd =topology->endChange();

    f_indices.beginEdit()->handleTopologyEvents(itBegin,itEnd,this->constrainedObject1->getSize());
}


template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::resetConstraint()
{
    core::behavior::LMConstraint<DataTypes,DataTypes>::resetConstraint();
    idxEquations.clear();
}

template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::buildConstraintMatrix(unsigned int &constraintId, core::VecId /*position*/)
{
    if (!idxEquations.empty()) return;

    MatrixDeriv& c = *this->constrainedObject1->getC();
    const SetIndexArray &indices = f_indices.getValue().getArray();
    const helper::vector<Deriv> &axis=BlockedAxis.getValue();
    idxEquations.resize(indices.size());
    unsigned int numParticle=0;
    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it, ++numParticle)
    {
        const unsigned int index=*it;
        for (unsigned int i=0; i<axis.size(); ++i)
        {
            c.writeLine(constraintId).addCol(index,axis[i]);
            idxEquations[numParticle].push_back(constraintId++);
        }
        this->constrainedObject1->forceMask.insertEntry(index);
    }
}


template<class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::writeConstraintEquations(unsigned int& lineNumber, VecId id, ConstOrder Order)
{

    typedef core::behavior::BaseMechanicalState::VecId VecId;
    //We don't constrain the Position, only the velocities and accelerations
    if (idxEquations.empty() ||
        Order==core::behavior::BaseLMConstraint::POS) return;


    const SetIndexArray & indices = f_indices.getValue().getArray();
    const helper::vector<SReal> &factor=factorAxis.getValue();

    for (unsigned int numParticle=0; numParticle<indices.size(); ++numParticle)
    {
        for (unsigned int i=0; i<idxEquations[numParticle].size(); ++i)
        {
            core::behavior::ConstraintGroup *constraint = this->addGroupConstraint(Order);
            SReal correction=0;
            switch(Order)
            {
            case core::behavior::BaseLMConstraint::ACC :
            case core::behavior::BaseLMConstraint::VEL :
            {
                correction = this->constrainedObject1->getConstraintJacobianTimesVecDeriv(idxEquations[numParticle][i],id);
                break;
            }
            default: break;
            };
            if (!factor.empty())
            {
                if (i < factor.size()) correction*=factor[i];
                else                   correction*=factor.back();
            }
            constraint->addConstraint( lineNumber, idxEquations[numParticle][i], -correction);
        }

    }

}



template <class DataTypes>
void DOFBlockerLMConstraint<DataTypes>::draw()
{
    if (!this->getContext()->getShowForceFields()) return;
    const VecCoord& x = *this->constrainedObject1->getX();

    const SetIndexArray & indices = f_indices.getValue().getArray();

    for (SetIndexArray::const_iterator it = indices.begin();
            it != indices.end();
            ++it)
    {
        unsigned int index=(*it);
        Coord pos=x[index];
        defaulttype::Vector3 position;
        DataTypes::get(position[0], position[1], position[2], pos);
        glColor3f(1,1,0);
        const helper::vector<Deriv>& axis=BlockedAxis.getValue();
        for (unsigned int i=0; i<axis.size(); ++i)
        {
            defaulttype::Vector3 direction;
            DataTypes::get(direction[0], direction[1], direction[2],axis[i]);
            helper::gl::Axis::draw(position,position+direction*showSizeAxis.getValue(),
                    showSizeAxis.getValue()*0.03);
        }

    }

}





} // namespace constraintset

} // namespace component

} // namespace sofa

#endif


