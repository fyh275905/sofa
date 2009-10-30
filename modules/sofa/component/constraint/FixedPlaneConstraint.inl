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
#ifndef SOFA_COMPONENT_CONSTRAINT_FIXEDPLANECONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_FIXEDPLANECONSTRAINT_INL

#include <sofa/core/componentmodel/behavior/Constraint.inl>
#include <sofa/component/constraint/FixedPlaneConstraint.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/helper/gl/template.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa
{

namespace component
{

namespace constraint
{
using namespace core::componentmodel::topology;
using namespace sofa::defaulttype;

template <class DataTypes>
FixedPlaneConstraint<DataTypes>::FixedPlaneConstraint()
    : indices( initData(&indices,"indices","Indices of the fixed points"))
    , direction( initData(&direction,"direction","normal direction of the plane"))
    , dmin( initData(&dmin,(Real)0,"dmin","Minimum plane distance from the origin"))
    , dmax( initData(&dmax,(Real)0,"dmax","Maximum plane distance from the origin") )
{
    selectVerticesFromPlanes=false;
}

template <class DataTypes>
FixedPlaneConstraint<DataTypes>::~FixedPlaneConstraint()
{
}
// Define TestNewPointFunction
template< class DataTypes>
bool FixedPlaneConstraint<DataTypes>::FPCTestNewPointFunction(int /*nbPoints*/, void* param, const helper::vector< unsigned int > &, const helper::vector< double >& )
{
    FixedPlaneConstraint<DataTypes> *fc= (FixedPlaneConstraint<DataTypes> *)param;
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
void FixedPlaneConstraint<DataTypes>::FPCRemovalFunction(int pointIndex, void* param)
{
    FixedPlaneConstraint<DataTypes> *fc= (FixedPlaneConstraint<DataTypes> *)param;
    if (fc)
    {
        fc->removeConstraint((unsigned int) pointIndex);
    }
    return;
}
template <class DataTypes>
FixedPlaneConstraint<DataTypes>*  FixedPlaneConstraint<DataTypes>::addConstraint(int index)
{
    indices.beginEdit()->push_back(index);
    return this;
}

template <class DataTypes>
FixedPlaneConstraint<DataTypes>*  FixedPlaneConstraint<DataTypes>::removeConstraint(int /*index*/)
{
    return this;
}

// -- Mass interface
template <class DataTypes>
void FixedPlaneConstraint<DataTypes>::projectResponse(VecDeriv& res)
{
    Coord dir=direction.getValue();

    for (helper::vector< unsigned int >::const_iterator it = this->indices.getValue().begin(); it != this->indices.getValue().end(); ++it)
    {
        /// only constraint one projection of the displacement to be zero
        res[*it]-= dir*dot(res[*it],dir);
    }
}

template <class DataTypes>
void FixedPlaneConstraint<DataTypes>::setDirection(Coord dir)
{
    if (dir.norm2()>0)
    {
        direction.setValue(dir);
    }
}

template <class DataTypes>
void FixedPlaneConstraint<DataTypes>::selectVerticesAlongPlane()
{
    VecCoord& x = *this->mstate->getX();
    unsigned int i;
    for(i=0; i<x.size(); ++i)
    {
        if (isPointInPlane(x[i]))
            addConstraint(i);
    }

}
template <class DataTypes>
void FixedPlaneConstraint<DataTypes>::init()
{
    this->core::componentmodel::behavior::Constraint<DataTypes>::init();

    topology = this->getContext()->getMeshTopology();

    /// test that dmin or dmax are different from zero
    if (dmin.getValue()!=dmax.getValue())
        selectVerticesFromPlanes=true;

    if (selectVerticesFromPlanes)
        selectVerticesAlongPlane();

    topology::PointSubset my_subset = indices.getValue();

    // Force the initialization of defined functions and parameters
    my_subset.setTestFunction(FPCTestNewPointFunction);
    my_subset.setRemovalFunction(FPCRemovalFunction);

    my_subset.setTestParameter( (void *) this );
    my_subset.setRemovalParameter( (void *) this );

}

// Handle topological changes
template <class DataTypes> void FixedPlaneConstraint<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=topology->lastChange();

    indices.beginEdit()->handleTopologyEvents(itBegin,itEnd,this->getMState()->getSize());

}

template <class DataTypes>
void FixedPlaneConstraint<DataTypes>::draw()
{
    if (!this->getContext()->getShowBehaviorModels()) return;
    const VecCoord& x = *this->mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,1.0,0.5,1);
    glBegin (GL_POINTS);
    for (helper::vector< unsigned int >::const_iterator it = this->indices.getValue().begin(); it != this->indices.getValue().end(); ++it)
    {
        helper::gl::glVertexT(x[*it]);
    }
    glEnd();
}

#ifndef SOFA_FLOAT
template <>
void FixedPlaneConstraint<Rigid3dTypes>::projectResponse(Rigid3dTypes::VecDeriv& /*res*/);

template <>
bool FixedPlaneConstraint<Rigid3dTypes>::isPointInPlane(Rigid3dTypes::Coord /*p*/);
#endif

#ifndef SOFA_DOUBLE
template <>
void FixedPlaneConstraint<Rigid3fTypes>::projectResponse(VecDeriv& /*res*/);

template <>
bool FixedPlaneConstraint<Rigid3fTypes>::isPointInPlane(Coord /*p*/);
#endif

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
