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
#ifndef SOFA_COMPONENT_CONSTRAINT_FIXEDCONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_FIXEDCONSTRAINT_INL

#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/componentmodel/behavior/Constraint.inl>
#include <sofa/component/constraint/FixedConstraint.h>
#include <sofa/component/topology/PointSubset.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/helper/gl/template.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <iostream>


#include <sofa/helper/gl/BasicShapes.h>




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


// Define TestNewPointFunction
template< class DataTypes>
bool FixedConstraint<DataTypes>::FCTestNewPointFunction(int /*nbPoints*/, void* param, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >& )
{
    FixedConstraint<DataTypes> *fc= (FixedConstraint<DataTypes> *)param;
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
void FixedConstraint<DataTypes>::FCRemovalFunction(int pointIndex, void* param)
{
    FixedConstraint<DataTypes> *fc= (FixedConstraint<DataTypes> *)param;
    if (fc)
    {
        fc->removeConstraint((unsigned int) pointIndex);
    }
    return;
}

template <class DataTypes>
FixedConstraint<DataTypes>::FixedConstraint()
    : core::componentmodel::behavior::Constraint<DataTypes>(NULL)
    , f_indices( initData(&f_indices,"indices","Indices of the fixed points") )
    , f_fixAll( initData(&f_fixAll,false,"fixAll","filter all the DOF to implement a fixed object") )
    , _drawSize( initData(&_drawSize,0.0,"drawSize","0 -> point based rendering, >0 -> radius of spheres") )
{
    // default to indice 0
    f_indices.beginEdit()->push_back(0);
    f_indices.endEdit();
}


// Handle topological changes
template <class DataTypes> void FixedConstraint<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=topology->lastChange();

    f_indices.beginEdit()->handleTopologyEvents(itBegin,itEnd,this->getMState()->getSize());

}

template <class DataTypes>
FixedConstraint<DataTypes>::~FixedConstraint()
{
}

template <class DataTypes>
void FixedConstraint<DataTypes>::clearConstraints()
{
    f_indices.beginEdit()->clear();
    f_indices.endEdit();
}

template <class DataTypes>
void FixedConstraint<DataTypes>::addConstraint(unsigned int index)
{
    f_indices.beginEdit()->push_back(index);
    f_indices.endEdit();
}

template <class DataTypes>
void FixedConstraint<DataTypes>::removeConstraint(unsigned int index)
{
    removeValue(*f_indices.beginEdit(),index);
    f_indices.endEdit();
}

// -- Constraint interface


template <class DataTypes>
void FixedConstraint<DataTypes>::init()
{
    this->core::componentmodel::behavior::Constraint<DataTypes>::init();

    topology = getContext()->getMeshTopology();

    // Initialize functions and parameters
    topology::PointSubset my_subset = f_indices.getValue();

    my_subset.setTestFunction(FCTestNewPointFunction);
    my_subset.setRemovalFunction(FCRemovalFunction);

    my_subset.setTestParameter( (void *) this );
    my_subset.setRemovalParameter( (void *) this );

}

template <class DataTypes>
void FixedConstraint<DataTypes>::projectResponse(VecDeriv& res)
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    //serr<<"FixedConstraint<DataTypes>::projectResponse, res.size()="<<res.size()<<sendl;
    if( f_fixAll.getValue()==true )    // fix everyting
    {
        for( unsigned i=0; i<res.size(); i++ )
            res[i] = Deriv();
    }
    else
    {
        for (SetIndexArray::const_iterator it = indices.begin();
                it != indices.end();
                ++it)
        {
            res[*it] = Deriv();
        }
    }
}

// Matrix Integration interface
template <class DataTypes>
void FixedConstraint<DataTypes>::applyConstraint(defaulttype::BaseMatrix *mat, unsigned int &offset)
{
    //sout << "applyConstraint in Matrix with offset = " << offset << sendl;
    const unsigned int N = Deriv::size();
    const SetIndexArray & indices = f_indices.getValue().getArray();

    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
    {
        // Reset Fixed Row and Col
        for (unsigned int c=0; c<N; ++c)
            mat->clearRowCol(offset + N * (*it) + c);
        // Set Fixed Vertex
        for (unsigned int c=0; c<N; ++c)
            mat->set(offset + N * (*it) + c, offset + N * (*it) + c, 1.0);
    }
}

template <class DataTypes>
void FixedConstraint<DataTypes>::applyConstraint(defaulttype::BaseVector *vect, unsigned int &offset)
{
    //sout << "applyConstraint in Vector with offset = " << offset << sendl;
    const unsigned int N = Deriv::size();

    const SetIndexArray & indices = f_indices.getValue().getArray();
    for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
    {
        for (unsigned int c=0; c<N; ++c)
            vect->clear(offset + N * (*it) + c);
    }
}

// Matrix Integration interface
template <class DataTypes>
void FixedConstraint<DataTypes>::applyInvMassConstraint(defaulttype::BaseVector *vec, unsigned int &offset)
{
    //sout << "applyConstraint in Matrix with offset = " << offset << sendl;
    //     const unsigned int N = Deriv::size();
    if (f_fixAll.getValue())
    {
        for (unsigned i=0; i<this->mstate->getSize(); i++ )
        {
            vec->clear(offset + i);
        }
    }
    else
    {
        const SetIndexArray & indices = f_indices.getValue().getArray();
        for (SetIndexArray::const_iterator it = indices.begin(); it != indices.end(); ++it)
        {
            vec->clear(offset +  (*it));
        }
    }
}

template <class DataTypes>
void FixedConstraint<DataTypes>::draw()
{
    if (!getContext()->
        getShowBehaviorModels()) return;
    if (!this->isActive()) return;
    const VecCoord& x = *this->mstate->getX();
    //serr<<"FixedConstraint<DataTypes>::draw(), x.size() = "<<x.size()<<sendl;




    const SetIndexArray & indices = f_indices.getValue().getArray();

    if( _drawSize.getValue() == 0) // old classical drawing by points
    {
        std::vector< Vector3 > points;
        Vector3 point;
        unsigned int sizePoints= (Coord::static_size <=3)?Coord::static_size:3;
        //serr<<"FixedConstraint<DataTypes>::draw(), indices = "<<indices<<sendl;
        if( f_fixAll.getValue()==true )
            for (unsigned i=0; i<x.size(); i++ )
            {
                for (unsigned int s=0; s<sizePoints; ++s) point[s] = x[i][s];
                points.push_back(point);
            }
        else
            for (SetIndexArray::const_iterator it = indices.begin();
                    it != indices.end();
                    ++it)
            {
                for (unsigned int s=0; s<sizePoints; ++s) point[s] = x[*it][s];
                points.push_back(point);
            }
        simulation::getSimulation()->DrawUtility.drawPoints(points, 10, Vec<4,float>(1,0.5,0.5,1));
    }
    else // new drawing by spheres
    {
        std::vector< Vector3 > points;
        Vector3 point;
        unsigned int sizePoints= (Coord::static_size <=3)?Coord::static_size:3;
        glColor4f (1.0f,0.35f,0.35f,1.0f);
        if( f_fixAll.getValue()==true )
            for (unsigned i=0; i<x.size(); i++ )
            {
                for (unsigned int s=0; s<sizePoints; ++s) point[s] = x[i][s];
                points.push_back(point);
            }
        else
            for (SetIndexArray::const_iterator it = indices.begin();
                    it != indices.end();
                    ++it)
            {
                for (unsigned int s=0; s<sizePoints; ++s) point[s] = x[*it][s];
                points.push_back(point);
            }
        simulation::getSimulation()->DrawUtility.drawSpheres(points, (float)_drawSize.getValue(), Vec<4,float>(1.0f,0.35f,0.35f,1.0f));

    }

}

// Specialization for rigids
#ifndef SOFA_FLOAT
template <>
void FixedConstraint<Rigid3dTypes >::draw();
template <>
void FixedConstraint<Rigid2dTypes >::draw();
#endif
#ifndef SOFA_DOUBLE
template <>
void FixedConstraint<Rigid3fTypes >::draw();
template <>
void FixedConstraint<Rigid2fTypes >::draw();
#endif



} // namespace constraint

} // namespace component

} // namespace sofa

#endif


