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

Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereModel> > RaySphereContactClass("default",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereModel> > RaySphereContactClass2("LagrangianMultiplier",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereModel> > RaySphereContactClass3("FrictionContact",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereModel> > RaySphereContactClass4("LDI",true);

Creator<core::componentmodel::collision::Contact::Factory, RayContact<TriangleModel> > RayTriangleContactClass("default",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<TriangleModel> > RayTriangleContactClass2("LagrangianMultiplier",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<TriangleModel> > RayTriangleContactClass3("FrictionContact",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<TriangleModel> > RayTriangleContactClass4("LDI",true);

Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereTreeModel> > RaySphereTreeContactClass("default",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereTreeModel> > RaySphereTreeContactClass2("LagrangianMultiplier",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereTreeModel> > RaySphereTreeContactClass3("FrictionContact",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<SphereTreeModel> > RaySphereTreeContactClass4("LDI",true);

Creator<core::componentmodel::collision::Contact::Factory, RayContact<RigidDistanceGridCollisionModel> > RayDistanceGridContactClass("default",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<RigidDistanceGridCollisionModel> > RayDistanceGridContactClass2("LagrangianMultiplier",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<RigidDistanceGridCollisionModel> > RayDistanceGridContactClass3("FrictionContact",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<RigidDistanceGridCollisionModel> > RayDistanceGridContactClass4("LDI",true);

Creator<core::componentmodel::collision::Contact::Factory, RayContact<FFDDistanceGridCollisionModel> > RayFFDDistanceGridContactClass("default",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<FFDDistanceGridCollisionModel> > RayFFDDistanceGridContactClass2("LagrangianMultiplier",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<FFDDistanceGridCollisionModel> > RayFFDDistanceGridContactClass3("FrictionContact",true);
Creator<core::componentmodel::collision::Contact::Factory, RayContact<FFDDistanceGridCollisionModel> > RayFFDDistanceGridContactClass4("LDI",true);

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
