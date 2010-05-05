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
#ifndef SOFA_COMPONENT_CONSTRAINT_FIXEDTRANSLATIONCONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_FIXEDTRANSLATIONCONSTRAINT_INL

#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/component/constraint/FixedTranslationConstraint.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/behavior/Constraint.inl>

namespace sofa
{

namespace component
{

namespace constraint
{

using namespace sofa::core::objectmodel;
using namespace sofa::core::behavior;
using namespace sofa::helper;

using namespace sofa::core::topology;

// Define TestNewPointFunction
template< class DataTypes>
bool FixedTranslationConstraint<DataTypes>::FCTestNewPointFunction(int /*nbPoints*/, void* param, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >& )
{
    FixedTranslationConstraint<DataTypes> *fc= (FixedTranslationConstraint<DataTypes> *)param;
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
void FixedTranslationConstraint<DataTypes>::FCRemovalFunction(int pointIndex, void* param)
{
    FixedTranslationConstraint<DataTypes> *fc= (FixedTranslationConstraint<DataTypes> *)param;
    if (fc)
    {
        fc->removeIndex((unsigned int) pointIndex);
    }
    return;
}

template< class DataTypes>
FixedTranslationConstraint<DataTypes>::FixedTranslationConstraint()
    : core::behavior::Constraint<DataTypes>(NULL)
    , f_indices( BaseObject::initData(&f_indices,"indices","Indices of the fixed points") )
    , f_fixAll( BaseObject::initData(&f_fixAll,false,"fixAll","filter all the DOF to implement a fixed object") )
    , _drawSize( BaseObject::initData(&_drawSize,0.0,"drawSize","0 -> point based rendering, >0 -> radius of spheres") )
{
    // default to indice 0
    f_indices.beginEdit()->push_back(0);
    f_indices.endEdit();
}

// Handle topological changes
template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=topology->lastChange();

    f_indices.beginEdit()->handleTopologyEvents(itBegin,itEnd,this->getMState()->getSize());

}

template <class DataTypes>
FixedTranslationConstraint<DataTypes>::~FixedTranslationConstraint()
{

}

template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::clearIndices()
{
    f_indices.beginEdit()->clear();
    f_indices.endEdit();
}

template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::addIndex(unsigned int index)
{
    f_indices.beginEdit()->push_back(index);
    f_indices.endEdit();
}

template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::removeIndex(unsigned int index)
{
    removeValue(*f_indices.beginEdit(),index);
    f_indices.endEdit();
}

// -- Constraint interface
template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::init()
{
    this->core::behavior::Constraint<DataTypes>::init();

    topology = this->getContext()->getMeshTopology();

    // Initialize functions and parameters
    topology::PointSubset my_subset = f_indices.getValue();

    my_subset.setTestFunction(FCTestNewPointFunction);
    my_subset.setRemovalFunction(FCRemovalFunction);

    my_subset.setTestParameter( (void *) this );
    my_subset.setRemovalParameter( (void *) this );

}// FixedTranslationConstraint::init


template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::projectResponse(VecDeriv& res)
{
    const SetIndexArray & indices = f_indices.getValue().getArray();

    if (f_fixAll.getValue() == true)
    {
        for( unsigned i = 0; i<res.size(); i++)
        {
            res[i].getVCenter() -= res[i].getVCenter();
        }
    }
    else
    {
        for(SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            res[*it].getVCenter() -= res[*it].getVCenter();
        }
    }
}// FixedTranslationConstraint::projectResponse

template <class DataTypes>
void FixedTranslationConstraint<DataTypes>::draw()
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    if (!this->getContext()->getShowBehaviorModels()) return;
    VecCoord& x = *this->mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    if( f_fixAll.getValue()==true )
    {
        for (unsigned i=0; i<x.size(); i++ )
        {
            gl::glVertexT(x[i].getCenter());
        }
    }
    else
    {
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            gl::glVertexT(x[*it].getCenter());
        }
    }
    glEnd();

}// FixedTranslationConstraint::draw


} // namespace constraint

} // namespace component

} // namespace sofa

#endif
