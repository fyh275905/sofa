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
#include <sofa/component/collision/BarycentricPenalityContact.inl>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace defaulttype;
using simulation::tree::GNode;

SOFA_DECL_CLASS(BarycentricPenalityContact)

Creator<Contact::Factory, BarycentricPenalityContact<SphereModel, SphereModel> > SphereSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<SphereModel, PointModel> > SpherePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<SphereTreeModel, SphereTreeModel> > SphereTreeSphereTreeContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<SphereTreeModel, TriangleModel> > SphereTreeTriangleContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<PointModel, PointModel> > PointPointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineModel, PointModel> > LinePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineModel, LineModel> > LineLinePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<LineModel, SphereModel> > LineSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleModel, SphereModel> > TriangleSpherePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleModel, PointModel> > TrianglePointPenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleModel, LineModel> > TriangleLinePenalityContactClass("default",true);
Creator<Contact::Factory, BarycentricPenalityContact<TriangleModel, TriangleModel> > TriangleTrianglePenalityContactClass("default",true);

Creator<Contact::Factory, BarycentricPenalityContact<RigidDistanceGridCollisionModel, RigidDistanceGridCollisionModel> > DistanceGridDistanceGridContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<RigidDistanceGridCollisionModel, PointModel> > DistanceGridPointContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<RigidDistanceGridCollisionModel, SphereModel> > DistanceGridSphereContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<RigidDistanceGridCollisionModel, TriangleModel> > DistanceGridTriangleContactClass("default", true);

Creator<Contact::Factory, BarycentricPenalityContact<FFDDistanceGridCollisionModel, FFDDistanceGridCollisionModel> > FFDDistanceGridContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<FFDDistanceGridCollisionModel, RigidDistanceGridCollisionModel> > FFDDistanceGridRigidDistanceGridContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<FFDDistanceGridCollisionModel, PointModel> > FFDDistanceGridPointContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<FFDDistanceGridCollisionModel, SphereModel> > FFDDistanceGridSphereContactClass("default", true);
Creator<Contact::Factory, BarycentricPenalityContact<FFDDistanceGridCollisionModel, TriangleModel> > FFDDistanceGridTriangleContactClass("default", true);


} // namespace collision

} // namespace component

} // namespace sofa

