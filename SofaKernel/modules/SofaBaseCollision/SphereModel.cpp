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
#define SOFA_COMPONENT_COLLISION_SPHERECOLLISIONMODEL_CPP
#include <SofaBaseCollision/SphereModel.inl>
#include <sofa/core/ObjectFactory.h>




namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;
using namespace sofa::core::collision;
using namespace helper;



template <> SOFA_BASE_COLLISION_API
Vector3 TSphere<defaulttype::Vec3Types >::getContactPointByNormal( const Vector3& )
{
    return center();
}
template <> SOFA_BASE_COLLISION_API
Vector3 TSphere<defaulttype::Vec3Types >::getContactPointWithSurfacePoint( const Vector3& )
{
    return center();
}




int SphereModelClass = core::RegisterObject("Collision model which represents a set of Spheres")
        .add<  SphereCollisionModel<Vec3Types> >()
        .add<SphereCollisionModel<Rigid3Types> >()

        .addAlias("TSphereModel")
        .addAlias("Sphere")
        .addAlias("SphereModel")
        ;

template class TSphere<defaulttype::Vec3Types>;
//template class TSphere<defaulttype::Rigid3Types>; // Can't compile due to type mismatches in pFree() method.
template class SphereCollisionModel<defaulttype::Vec3Types>;
template class SphereCollisionModel<defaulttype::Rigid3Types>;


} // namespace collision

} // namespace component

} // namespace sofa

