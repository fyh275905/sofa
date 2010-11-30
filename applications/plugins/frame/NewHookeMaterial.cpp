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
#include "NewHookeMaterial.inl"
#include <sofa/core/ObjectFactory.h>
#include "AffineTypes.h"
#include "QuadraticTypes.h"


namespace sofa
{
namespace component
{
namespace material
{
using namespace sofa::defaulttype;

SOFA_DECL_CLASS (HookeMaterial3);
// Register in the Factory

int HookeMaterial3Class = core::RegisterObject ( "Hooke material for deformable objects" )
#ifndef SOFA_FLOAT
        .add<HookeMaterial3<Rigid3dTypes> >()
        .add<HookeMaterial3<Affine3dTypes> >()
        .add<HookeMaterial3<Quadratic3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add<HookeMaterial3<Rigid3fTypes> >()
        .add<HookeMaterial3<Affine3fTypes> >()
        .add<HookeMaterial3<Quadratic3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_FRAME_API HookeMaterial3<Rigid3dTypes>;
template class SOFA_FRAME_API HookeMaterial3<Affine3dTypes>;
template class SOFA_FRAME_API HookeMaterial3<Quadratic3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_FRAME_API HookeMaterial3<Rigid3fTypes>;
template class SOFA_FRAME_API HookeMaterial3<Affine3fTypes>;
template class SOFA_FRAME_API HookeMaterial3<Quadratic3fTypes>;
#endif



}

} // namespace component

} // namespace sofa

