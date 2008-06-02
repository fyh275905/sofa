/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#include <sofa/component/collision/RayContact.h>
#include <sofa/component/collision/RayModel.h>
#include <sofa/component/collision/SphereModel.h>
#include <sofa/component/collision/TriangleModel.h>
#include <sofa/component/collision/TetrahedronModel.h>
#include <sofa/component/collision/SphereTreeModel.h>
#include <sofa/component/collision/DistanceGridCollisionModel.h>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

SOFA_DECL_CLASS(RayContact)

Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereModel> > RaySphereContactClass("ray",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<TriangleModel> > RayTriangleContactClass("ray",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<TetrahedronModel> > RayTetrahedronContactClass("ray",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereTreeModel> > RaySphereTreeContactClass("ray",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<RigidDistanceGridCollisionModel> > RayDistanceGridContactClass("ray",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<FFDDistanceGridCollisionModel> > RayFFDDistanceGridContactClass("ray",true);

BaseRayContact::BaseRayContact(CollisionModel1* model1, core::componentmodel::collision::Intersection* /*instersectionMethod*/)
    : model1(model1)
{
    if (model1!=NULL)
        model1->addContact(this);
}

BaseRayContact::~BaseRayContact()
{
    if (model1!=NULL)
        model1->removeContact(this);
}


} // namespace collision

} // namespace component

} // namespace sofa
