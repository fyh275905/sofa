/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#define SOFA_SOFAMISCCOLLISION_CAPSULECONTACTMAPPER_CPP
#include <SofaMiscCollision/CapsuleContactMapper.h>

#include <SofaMeshCollision/BarycentricContactMapper.inl>
#include <SofaMeshCollision/RigidContactMapper.inl>
#include <SofaBaseCollision/CapsuleModel.h>
#include <SofaBaseCollision/RigidCapsuleModel.h>

using namespace sofa::core::collision;

namespace sofa::component::collision
{

ContactMapperCreator< ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>> > CapsuleContactMapperClass("default", true);
ContactMapperCreator< ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Rigid3Types>, sofa::defaulttype::Vec3Types> > RigidCapsuleContactMapperClass("default", true);
template class SOFA_MISC_COLLISION_API ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;
template class SOFA_MISC_COLLISION_API ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Rigid3Types>, sofa::defaulttype::Vec3Types>;

} // namespace sofa::component::collision
