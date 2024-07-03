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
#include <sofa/component/collision/response/contact/config.h>
#include <sofa/component/collision/response/contact/FrictionContact.inl>
#include <sofa/component/collision/response/mapper/BarycentricContactMapper.h>
#include <sofa/component/collision/response/mapper/TetrahedronBarycentricContactMapper.h>
#include <sofa/component/collision/geometry/TetrahedronModel.h>

using namespace sofa::core::collision;

namespace sofa::component::collision::response::contact
{

using namespace sofa::component::collision::geometry;

Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronSphereFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronPointFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronLineFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronTriangleFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::COULOMB> > TetrahedronTetrahedronFrictionContactClass("FrictionContactConstraint",true);

Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronSpherePenalityFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronPointPenalityFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronLinePenalityFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB> > TetrahedronTrianglePenalityFrictionContactClass("FrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::COULOMB> > TetrahedronTetrahedronPenalityFrictionContactClass("FrictionContactConstraint",true);

template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::COULOMB>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::COULOMB>;


Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronSphereViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronPointViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronLineViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronTriangleViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::VISCOUS> > TetrahedronTetrahedronViscousFrictionContactClass("ViscousFrictionContactConstraint",true);

Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronSpherePenalityViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronPointPenalityViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronLinePenalityViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS> > TetrahedronTrianglePenalityViscousFrictionContactClass("ViscousFrictionContactConstraint",true);
Creator<Contact::Factory, FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::VISCOUS> > TetrahedronTetrahedronPenalityViscousFrictionContactClass("ViscousFrictionContactConstraint",true);

template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, SphereCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, PointCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, LineCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, TriangleCollisionModel<sofa::defaulttype::Vec3Types>,FrictionType::VISCOUS>;
template class SOFA_COMPONENT_COLLISION_RESPONSE_CONTACT_API response::contact::FrictionContact<TetrahedronCollisionModel, TetrahedronCollisionModel,FrictionType::VISCOUS>;

}  // namespace sofa::component::collision::response::contact
