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
#include <sofa/component/constraint/FixedConstraint.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/RigidTypes.h>

#include <sofa/simulation/tree/GNode.h>
#include <sofa/component/mass/UniformMass.h>

namespace sofa
{

namespace component
{

namespace constraint
{

using namespace sofa::defaulttype;
using namespace sofa::helper;


SOFA_DECL_CLASS(FixedConstraint)

int FixedConstraintClass = core::RegisterObject("Attach given particles to their initial positions")
#ifndef SOFA_FLOAT
        .add< FixedConstraint<Vec3dTypes> >()
        .add< FixedConstraint<Vec2dTypes> >()
        .add< FixedConstraint<Vec1dTypes> >()
        .add< FixedConstraint<Vec6dTypes> >()
        .add< FixedConstraint<Rigid3dTypes> >()
        .add< FixedConstraint<Rigid2dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< FixedConstraint<Vec3fTypes> >()
        .add< FixedConstraint<Vec2fTypes> >()
        .add< FixedConstraint<Vec1fTypes> >()
        .add< FixedConstraint<Vec6fTypes> >()
        .add< FixedConstraint<Rigid3fTypes> >()
        .add< FixedConstraint<Rigid2fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class FixedConstraint<Vec3dTypes>;
template class FixedConstraint<Vec2dTypes>;
template class FixedConstraint<Vec1dTypes>;
template class FixedConstraint<Vec6dTypes>;
template class FixedConstraint<Rigid3dTypes>;
template class FixedConstraint<Rigid2dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class FixedConstraint<Vec3fTypes>;
template class FixedConstraint<Vec2fTypes>;
template class FixedConstraint<Vec1fTypes>;
template class FixedConstraint<Vec6fTypes>;
template class FixedConstraint<Rigid3fTypes>;
template class FixedConstraint<Rigid2fTypes>;
#endif

#ifndef SOFA_FLOAT
template <>
void FixedConstraint<Rigid3dTypes>::draw()
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    if (!getContext()->getShowBehaviorModels()) return;
    VecCoord& x = *mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    if( f_fixAll.getValue()==true )
        for (unsigned i=0; i<x.size(); i++ )
            gl::glVertexT(x[i].getCenter());
    else
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
            gl::glVertexT(x[*it].getCenter());
    glEnd();
}

template <>
void FixedConstraint<Rigid2dTypes>::draw()
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    if (!getContext()->getShowBehaviorModels()) return;
    VecCoord& x = *mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    if( f_fixAll.getValue()==true )
        for (unsigned i=0; i<x.size(); i++ )
            gl::glVertexT(x[i].getCenter());
    else
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
            gl::glVertexT(x[*it].getCenter());
    glEnd();
}
#endif

#ifndef SOFA_DOUBLE
template <>
void FixedConstraint<Rigid3fTypes>::draw()
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    if (!getContext()->getShowBehaviorModels()) return;
    VecCoord& x = *mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    if( f_fixAll.getValue()==true )
        for (unsigned i=0; i<x.size(); i++ )
            gl::glVertexT(x[i].getCenter());
    else
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
            gl::glVertexT(x[*it].getCenter());
    glEnd();
}

template <>
void FixedConstraint<Rigid2fTypes>::draw()
{
    const SetIndexArray & indices = f_indices.getValue().getArray();
    if (!getContext()->getShowBehaviorModels()) return;
    VecCoord& x = *mstate->getX();
    glDisable (GL_LIGHTING);
    glPointSize(10);
    glColor4f (1,0.5,0.5,1);
    glBegin (GL_POINTS);
    if( f_fixAll.getValue()==true )
        for (unsigned i=0; i<x.size(); i++ )
            gl::glVertexT(x[i].getCenter());
    else
        for (SetIndex::const_iterator it = indices.begin(); it != indices.end(); ++it)
            gl::glVertexT(x[*it].getCenter());
    glEnd();
}
#endif



} // namespace constraint

} // namespace component

} // namespace sofa

