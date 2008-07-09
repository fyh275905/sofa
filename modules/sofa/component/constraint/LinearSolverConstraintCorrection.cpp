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
#include "LinearSolverConstraintCorrection.inl"
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{
namespace component
{
namespace constraint
{
using namespace sofa::defaulttype;

SOFA_DECL_CLASS(LinearSolverConstraintCorrection)

int LinearSolverContactCorrectionClass = core::RegisterObject("")
#ifndef SOFA_FLOAT
        .add< LinearSolverConstraintCorrection<Vec3dTypes> >()
        .add< LinearSolverConstraintCorrection<Vec1dTypes> >()
        .add< LinearSolverConstraintCorrection<Rigid3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< LinearSolverConstraintCorrection<Vec3fTypes> >()
        .add< LinearSolverConstraintCorrection<Vec1fTypes> >()
        .add< LinearSolverConstraintCorrection<Rigid3fTypes> >()
#endif
        ;
#ifndef SOFA_FLOAT
template class LinearSolverConstraintCorrection<Vec3dTypes>;
template class LinearSolverConstraintCorrection<Vec1dTypes>;
template class LinearSolverConstraintCorrection<Rigid3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class LinearSolverConstraintCorrection<Vec3fTypes>;
template class LinearSolverConstraintCorrection<Vec1fTypes>;
template class LinearSolverConstraintCorrection<Rigid3fTypes>;
#endif
//template class LinearSolverConstraintCorrection<Vec3fTypes>;
//template class LinearSolverConstraintCorrection<Vec2dTypes>;
//template class LinearSolverConstraintCorrection<Vec2fTypes>;
//template class LinearSolverConstraintCorrection<Vec1fTypes>;
//template class LinearSolverConstraintCorrection<Rigid3fTypes>;
//template class LinearSolverConstraintCorrection<Rigid2dTypes>;
//template class LinearSolverConstraintCorrection<Rigid2fTypes>;


} // namespace collision

} // namespace component

} // namespace sofa
